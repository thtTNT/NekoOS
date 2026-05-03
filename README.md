# NekoOS

A toy operating system for RISC-V, written in C++ and assembly. Runs on the `virt` machine in QEMU with OpenSBI firmware in S-mode.

## Features

- **RISC-V S-mode kernel** with M-mode → S-mode boot via OpenSBI
- **Page frame allocator** and **page table** management (Sv39)
- **Kernel heap** with `malloc`/`free`
- **Trap handling** — S-mode interrupts (timer, PLIC) with `sscratch`-based per-mode stack switching
- **Process scheduler** — round-robin with timer interrupts, supports kernel and user processes
- **User-mode processes** — independent page tables, U-mode return, SATP switching
- **VirtIO block device** — disk discovery, read/write
- **Custom filesystem** — generated via `filesystem/makeFileSystem.js`

## Quick Start (Docker)

The recommended development environment is Docker. All dependencies are pre-installed in the container image.

```bash
# Build the image and start the container
docker compose up -d

# Enter the container
docker compose exec nekoos bash

# Inside the container:
make              # build the kernel
make filesystem   # generate disk image
make qemu         # run in QEMU
```

## Build

### Prerequisites

If building without Docker, you need:

- `riscv64-unknown-elf-gcc` (cross-compiler)
- `riscv64-unknown-elf-ld` (cross-linker)
- `qemu-system-riscv64`
- `node` (for filesystem image generation)
- `make`

### Makefile Targets

| Target | Description |
|--------|-------------|
| `make` / `make all` | Build the kernel ELF at `build/kernel` |
| `make filesystem` | Generate `build/disk.img` |
| `make qemu` | Build and run in QEMU (`-nographic`) |
| `make qemu-gdb` | Build and run with GDB stub (port 1234) |

### Debugging

```bash
# Terminal 1 — start QEMU with GDB stub
make qemu-gdb

# Terminal 2 — connect GDB
gdb-multiarch build/kernel -ex "target remote :1234"
```

## Architecture

```
src/
├── entry.S          # Boot entry point and trap vector
├── start.cpp        # Early initialization (M → S mode transition)
├── main.cpp         # Kernel entry (main0)
├── kernel.h         # cpuid helper
├── driver/          # VirtIO block device, UART rendering
├── memory/          # Page frame allocator, page tables, heap
├── process/         # Scheduler, process creation (kernel/user)
├── trap/            # PLIC, trap dispatch
└── utils/           # RISC-V CSR helpers, linked list
```

The kernel is linked at `0x80200000` (the standard OpenSBI S-mode entry point). It uses a `kernel.ld` linker script that places `.text`, `.data`, `.rodata`, and `.bss` sections with 4 KiB alignment.

## Project Structure

```
.
├── Dockerfile              # Ubuntu 24.04 + RISC-V toolchain + QEMU
├── docker-compose.yml      # Mounts workspace into container
├── Makefile                # Build and run targets
├── kernel.ld               # Linker script
├── filesystem/
│   └── makeFileSystem.js   # Disk image generator
└── src/                    # Kernel source
```
