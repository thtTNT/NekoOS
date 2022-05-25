.PHONY: qemu qemu-gdb

SRC_DIR=src
OBJ_DIR=build/bin
BUILD_DIR=build
LD=riscv64-unknown-elf-ld
CC=riscv64-unknown-elf-gcc
NODE = node
CFLAGS=-ffreestanding -fshort-wchar -fno-exceptions -mcmodel=medany -std=gnu++17 -nostdlib -c -ggdb
LDFLAGS = -z max-page-size=4096

SRC = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/**/*.cpp)
ASM_SRC = $(wildcard $(SRC_DIR)/*.S)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
ASM_OBJS = $(patsubst $(SRC_DIR)/%.S, $(OBJ_DIR)/%_asm.o, $(ASM_SRC))

$(BUILD_DIR)/kernel: $(OBJS) $(ASM_OBJS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(ASM_OBJS) $(OBJS)

all: $(BUILD_DIR)/kernel

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%_asm.o: $(SRC_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

# Filesystem
export IMAGE_PATH = build/disk.img
export IMAGE_SIZE = 33554432

filesystem: $(IMAGE_PATH)

$(IMAGE_PATH):
	$(NODE) filesystem/makeFileSystem.js

# GDB
QEMU_OPTIONS = -machine virt -bios none -kernel $(BUILD_DIR)/kernel -m 128M -smp 1 -nographic
QEMU_OPTIONS += -drive file=$(IMAGE_PATH),if=none,format=raw,id=x0
QEMU_OPTIONS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu: $(BUILD_DIR)/kernel
	@clear
	@qemu-system-riscv64 $(QEMU_OPTIONS)

qemu-gdb: $(BUILD_DIR)/kernel
	qemu-system-riscv64 $(QEMU_OPTIONS) -S -s