# CLAUDE.md

This file provides guidance to Claude Code when working with code in
this repository.

## Project Overview

Swift 6 bridge library that wraps the NdsSDFormat C++ FAT32
formatting library for consumption by Swift applications. Provides
type-safe, RAII-managed Swift wrappers around the C bridge API.

This is **Repo 2** of a 3-repo dependency chain:

```text
herbderby/NDSFlashcartFormatter (App)
  depends on -> herbderby/SwiftNdsSdFormat (this repo, Swift bridge)
    depends on -> herbderby/NdsSDFormat (C++ library)
```

SPM product: `NDSSDFormat`

## Build Commands

```bash
swift build            # Debug build
swift build -c release # Release build
```

No tests defined yet. Verification is done downstream by the
application repo's integration tests.

## Directory Structure

```text
SwiftNdsSdFormat/
├── Sources/
│   ├── CSDFormat/
│   │   ├── include/
│   │   │   ├── SDFormatBridge.h      # C bridge header
│   │   │   └── module.modulemap      # Clang module map
│   │   └── SDFormatBridge.cpp        # C bridge implementation
│   └── NDSSDFormat/
│       ├── SectorWriter.swift        # ~Copyable RAII wrapper
│       └── FormatterError.swift      # Error enum
└── Package.swift
```

## Architecture

Two SPM targets:

**CSDFormat** (C++ target) — The `extern "C"` bridge layer.

- `SDFormatBridge.h` — Opaque context handle
  (`SDFormatContextRef`) with C functions: `SDFormat_CreateContext`,
  `SDFormat_DestroyContext`, `SDFormat_WriteMBR`, etc.
- `SDFormatBridge.cpp` — Implementation that delegates to
  `sdFormat::SectorWriter` from NdsSDFormat.
- Depends on `NDSSDFormatCore` (Repo 1) for C++ headers and
  implementation.

**NDSSDFormat** (Swift target) — Public Swift API.

- `SectorWriter` — `~Copyable` struct (RAII semantics). Wraps
  `SDFormatContextRef`, destroys context in `deinit`. Public
  methods: `writeMasterBootRecord()`,
  `writeVolumeBootRecord()`, `writeFileSystemInfo()`,
  `writeFAT32()`, `writeRootDirectory()`.
- `FormatterError` — `Error` enum mapping C result codes to
  Swift errors. Cases: `accessDenied`, `deviceBusy`,
  `invalidDevice`, `ioError`, `tooSmall`, `unknown(UInt32)`.

Key principle: **All types are `public`**. This library exists
solely to be consumed by downstream packages.

## Engineering Standards

See `~/.claude/standards/` for shared conventions:

- `base.md` — Core principles (dead code deletion, atomic commits)
- `swift.md` — Swift 6 standards (concurrency, API design, ~Copyable)
- `cpp.md` — C++23 standards (for the bridge layer)
- `git.md` — Commit workflow (no `git add .`, message file, prefixes)
- `markdown.md` — Formatting rules (80-char wrap, 2-space indent)

### Project-Specific

- Use `~Copyable` for RAII semantics (move-only wrappers)
- Convert C result codes to `throws` at the bridge boundary
- C++ bridge uses `.unsafeFlags(["-std=c++23"])` in cxxSettings

## SPM Notes

- Depends on NdsSDFormat via
  `.package(url: "https://github.com/herbderby/NdsSDFormat.git",
  from: "2.0.0")`.
- The `CSDFormat` target's `publicHeadersPath` is
  `Sources/CSDFormat/include`, which exposes `SDFormatBridge.h`
  and the `module.modulemap`.
- Never re-tag a released version. Always bump version numbers.
- When updating the NdsSDFormat dependency version, also delete
  `.build/checkouts/NdsSDFormat` if SPM caches a stale checkout.
- **NdsSDFormat 2.0.0 breaking change**: The C++ method
  `writeFat32BootSector()` was renamed to
  `writeVolumeBootRecord()`. The C bridge
  (`SDFormatBridge.cpp`) and Swift wrapper
  (`SectorWriter.swift`) must be updated to match:
  `writeBootSector()` → `writeVolumeBootRecord()`.

## Next Steps

1. Add unit tests for error code mapping
2. Document public APIs with DocC comments
