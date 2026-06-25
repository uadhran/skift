from cutekit import cli, model

_REPLACED_ON_KERNEL = {
    "karm-math": "karm-math.hjert",
}

# Userspace stacks that become enabled once karm-sys resolves on kernel.
_KERNEL_DISABLE_PREFIXES = (
    "ambolt",
    "games",
    "hideo",
    "luna",
    "vaev",
    "karm-app",
    "karm-av",
    "karm-archive",
    "karm-cli",
    "karm-debug",
    "karm-diag",
    "karm-dns",
    "karm-font",
    "karm-fs",
    "karm-gfx",
    "karm-http",
    "karm-icu",
    "karm-idl",
    "karm-image",
    "karm-kv",
    "karm-md",
    "karm-ml",
    "karm-pdf",
    "karm-print",
    "karm-scene",
    "karm-template",
    "karm-tls",
    "karm-test",
    "strata",
    "utilities",
)


_KERNEL_EXECUTABLES = {
    "hjert",
    "opstart",
}


def _shouldDisableOnKernel(component: model.Component) -> bool:
    if ".tests" in component.id or component.id.endswith(".test"):
        return True
    if component.type == model.Kind.EXE and component.id not in _KERNEL_EXECUTABLES:
        return True
    return any(
        component.id == prefix
        or component.id.startswith(f"{prefix}.")
        or component.id.startswith(f"{prefix}-")
        for prefix in _KERNEL_DISABLE_PREFIXES
    )


def _patchKernelTargets(registry: model.Registry) -> None:
    for target in registry.iter(model.Target):
        if target.props.get("sys") != "kernel":
            continue

        for replaced, replacement in _REPLACED_ON_KERNEL.items():
            stub = registry.lookup(replacement, model.Component)
            original = registry.lookup(replaced, model.Component)
            if stub is None or original is None:
                continue
            if not stub.resolved[target.id].enabled:
                continue
            original.resolved[target.id] = model.Resolved(
                reason=f"replaced by {replacement} on kernel targets"
            )

        for component in registry.iter(model.Component):
            resolved = component.resolved[target.id]
            if not resolved.enabled:
                continue
            if not _shouldDisableOnKernel(component):
                continue
            component.resolved[target.id] = model.Resolved(
                reason="not required on kernel targets"
            )


def _prepareKernelComponents(registry: model.Registry) -> None:
    ipc = registry.lookup("karm-ipc", model.Component)
    if ipc is not None and "karm-sys" not in ipc.requires:
        ipc.requires = [*ipc.requires, "karm-sys"]


_origLoadDependencies = model.Registry._loadDependencies


@staticmethod
def _loadDependencies(registry, mixins, props):
    _prepareKernelComponents(registry)
    _origLoadDependencies(registry, mixins, props)
    _patchKernelTargets(registry)


model.Registry._loadDependencies = _loadDependencies


def _isKernelBuild(args) -> bool:
    target = getattr(args, "target", "")
    return isinstance(target, str) and target.startswith("kernel")


def _wrapBuildCommand() -> None:
    command = cli._resolvePath(cli._splitPath("build"))
    original = command.callable

    def wrapper(args):
        attempts = (
            2
            if _isKernelBuild(args)
            and not getattr(args, "database", False)
            and not getattr(args, "universe", False)
            else 1
        )
        lastError: Exception | None = None
        for attempt in range(attempts):
            try:
                return original(args)
            except Exception as error:
                lastError = error
                if attempt + 1 >= attempts:
                    raise
        if lastError is not None:
            raise lastError

    command.callable = wrapper


_wrapBuildCommand()