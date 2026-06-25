module Karm.Sys;

import Karm.Core;
import Karm.Ref;

namespace Karm::Sys::_Embed {

Res<Rc<Fd>> deserializeFd(Serde::Deserializer&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> openFile(Ref::Url const&, Flags<OpenOption>) {
    return Error::notImplemented();
}

Res<Pair<Rc<Fd>, Rc<Fd>>> createPipe() {
    return Error::notImplemented();
}

Res<Rc<Fd>> createIn() {
    return Error::notImplemented();
}

Res<Rc<Fd>> createOut() {
    return Error::notImplemented();
}

Res<Rc<Fd>> createErr() {
    return Error::notImplemented();
}

Res<Vec<DirEntry>> readDir(Ref::Url const&) {
    return Error::notImplemented();
}

Res<> createDir(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Vec<DirEntry>> readDirOrCreate(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Stat> stat(Ref::Url const&) {
    return Error::notImplemented();
}

Res<> launch(Intent) {
    return Error::notImplemented();
}

Async::Task<> launchAsync(Intent) {
    co_return Error::notImplemented();
}

Res<Rc<Pid>> spawn(Command const&) {
    return Error::notImplemented();
}

Res<Tuple<Rc<Pid>, Rc<Fd>>> spawnPty(Command const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenUdp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> connectTcp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenTcp(SocketAddr) {
    return Error::notImplemented();
}

Res<_Connected> connectIpc(Ref::Url) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenIpc(Ref::Url) {
    return Error::notImplemented();
}

SystemTime now() {
    return SystemTime::epoch();
}

Instant instant() {
    return Instant::epoch();
}

Duration uptime() {
    notImplemented();
}

Res<MmapResult> memMap(MmapProps const&) {
    return Error::notImplemented();
}

Res<MmapResult> memMap(MmapProps const&, Rc<Fd>) {
    return Error::notImplemented();
}

Res<> memUnmap(void const*, usize) {
    return Error::notImplemented();
}

Res<> memFlush(void*, usize) {
    return Error::notImplemented();
}

usize pageSize() {
    return 0x1000;
}

Res<> populate(SysInfo&) {
    return Error::notImplemented();
}

Res<> populate(MemInfo&) {
    return Error::notImplemented();
}

Res<> populate(Vec<CpuInfo>&) {
    return Error::notImplemented();
}

Res<> populate(UserInfo&) {
    return Error::notImplemented();
}

Res<> populate(Vec<UserInfo>&) {
    return Error::notImplemented();
}

Res<> sleep(Duration) {
    return Error::notImplemented();
}

Res<> sleepUntil(Instant) {
    return Error::notImplemented();
}

Res<> exit(i32) {
    while (true)
        ;
}

Res<> hardenSandbox() {
    return Ok();
}

Async::Task<Vec<Ip>> ipLookupAsync(Str) {
    co_return Error::notImplemented();
}

Res<Vec<String>> installedBundles() {
    return Error::notImplemented();
}

Res<String> currentBundle() {
    return Error::notImplemented();
}

} // namespace Karm::Sys::_Embed