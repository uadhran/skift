#include <hal/mem.h>
#include <vaerk-handover/builder.h>

import Karm.Core;
import Karm.Logger;
import Vaerk.Sbi;
import Vaerk.Riscv;
import Vaerk.Dtb;
import Hjert.Core;

using namespace Karm;
using namespace Karm::Literals;
using namespace Vaerk;

extern "C" char __bss_start[], __bss_end[];
extern "C" char __kernel_phys_start[], __kernel_phys_end[];

namespace Hjert {

void stop() {
    for (;;)
        Riscv::wfi();
}

static_assert(sizeof(usize) == sizeof(u64));

urange handoverPayloadRange() {
    auto start = alignUp(reinterpret_cast<usize>(__kernel_phys_end), 4096);
    auto end = start + 16_KiB;
    return urange::fromStartEnd(start, end);
}

MutBytes handoverPayloadSlice() {
    auto range = handoverPayloadRange();
    return {
        (u8*)(range.start + Hal::UPPER_HALF),
        range.size
    };
}

void buildMemoryMap(Dtb::Blob& blob, Handover::Builder& builder) {
    auto root = blob.root();

    for (auto memory : root.iterChildrenByType("memory")) {
        auto reg = memory.getProperty<Dtb::AddressSizeCell>("reg");
        builder.add(Handover::FREE, 0, reg->cast<usize>());
    }

    if (auto [reservedMemory] = root.findChildren("reserved-memory")) {
        for (auto reserved : reservedMemory.iterChildren()) {
            bool noMap = reserved.getProperty("no-map").has();
            auto reg = reserved.getProperty<Dtb::AddressSizeCell>("reg");
            builder.add(noMap ? Handover::RESERVED : Handover::LOADER, 0, reg->cast<usize>());
        }
    }

    urange kernelRange = urange::fromStartEnd(
        reinterpret_cast<usize>(__kernel_phys_start),
        alignUp(reinterpret_cast<usize>(__kernel_phys_end), 4096)
    );
    builder.add(Handover::KERNEL, 0, kernelRange);
    if (auto [initrdRange] = blob.initrd())
        builder.add(Handover::BLOB, 0, initrdRange);
    builder.add(Handover::SELF, 0, handoverPayloadRange());
}

void entry(usize hart, usize dtbPtr) {
    (void)hart;
    auto dtb = Dtb::Blob::openFromAddr((void*)(dtbPtr + Hal::UPPER_HALF)).unwrap();
    Io::Emit e = Arch::globalOut();
    dtb.dump(e);

    Handover::Builder builder{handoverPayloadSlice()};
    builder.agent("OpenSBI");
    buildMemoryMap(dtb, builder);
    builder.add(
        Handover::FDT,
        0,
        {
            dtbPtr,
            alignUp(dtb.header().totalSize, 4096),
        }
    );

    auto& payload = builder.finalize();
    (void)Core::init(Handover::COOLBOOT, payload);
}

} // namespace Hjert

void __panicHandler(PanicKind kind, char const* buf, usize len) {
    if (kind == PanicKind::PANIC) {
        Sbi::consolePuts("panic: "s);
        Sbi::consolePuts(Str{buf, len});
        Sbi::consolePuts("\n");
        Hjert::stop();
        __builtin_unreachable();
    } else {
        Sbi::consolePuts("debug: "s);
        Sbi::consolePuts(Str{buf, len});
        Sbi::consolePuts("\n");
    }
}

extern "C" void _hjertEntry(usize hart, usize dtbPtr) {
    std::memset(__bss_start, 0, reinterpret_cast<usize>(__bss_end) - reinterpret_cast<usize>(__bss_start));
    registerPanicHandler(__panicHandler);
    Hjert::entry(hart, dtbPtr);
    unreachable();
}
