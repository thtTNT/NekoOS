FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    # RISC-V cross compiler (g++ includes C++ headers)
    gcc-riscv64-unknown-elf \
    # RISC-V binutils (ld, objdump, etc.)
    binutils-riscv64-unknown-elf \
    # QEMU for RISC-V
    qemu-system-riscv64 \
    # FAT filesystem tools (mkfs.fat, mcopy)
    dosfstools \
    mtools \
    # General dev tools
    make \
    gdb-multiarch \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["/bin/bash"]
