.section .bss
.align 16
.global __stack_top
.global __stack_bottom
__stack_top:
    .skip 0x20000
__stack_bottom:

.section .text.boot
.global _kstart
_kstart:
    mv ra, zero
    mv fp, zero

    lla t0, boot_page_table

    li t1, 0x80000
    slli t1, t1, 10
    ori t1, t1, 0xCF
    sd t1, 16(t0)

    li t3, 4080
    add t3, t0, t3
    sd t1, 0(t3)

    srli t0, t0, 12
    li t2, 8
    slli t2, t2, 60
    or t0, t0, t2

    csrw satp, t0
    sfence.vma

    lla t0, .trampoline_ptr
    ld t0, 0(t0)
    jr t0

.align 3
.trampoline_ptr:
    .quad .trampoline

.align 12
boot_page_table:
    .zero 4096

.section .text
.trampoline:
    mv ra, zero
    mv fp, zero

    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    la sp, __stack_bottom

    la t0, _hjertEntry
    jr t0