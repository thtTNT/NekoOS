.PHONY: qemu qemu-gdb docker-build docker-up docker-enter docker-down filesystem

SRC_DIR=src
OBJ_DIR=build/bin
BUILD_DIR=build
LD=riscv64-unknown-elf-ld
CC=riscv64-unknown-elf-gcc
CFLAGS=-ffreestanding -fshort-wchar -no-pie -fno-exceptions -mcmodel=medany -std=gnu++17 -nostdlib -c -ggdb -Wall -Werror -Isrc/libc
LDFLAGS = -z max-page-size=4096
IMAGE_PATH=build/disk.img

SRC = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/**/*.cpp)
ASM_SRC = $(wildcard $(SRC_DIR)/*.S $(SRC_DIR)/**/*.S)
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
$(IMAGE_PATH):
	@mkdir -p $(BUILD_DIR)
	dd if=/dev/zero of=$@ bs=1M count=32
	mkfs.fat -F 32 $@

filesystem: $(IMAGE_PATH)

# QEMU
QEMU_OPTIONS = -machine virt -kernel $(BUILD_DIR)/kernel -m 128M -smp 1 -nographic
QEMU_OPTIONS += -drive file=$(IMAGE_PATH),if=none,format=raw,id=x0
QEMU_OPTIONS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu: $(BUILD_DIR)/kernel
	@clear
	@qemu-system-riscv64 $(QEMU_OPTIONS)

qemu-gdb: $(BUILD_DIR)/kernel
	qemu-system-riscv64 $(QEMU_OPTIONS) -S -s

# Docker
docker-build:
	docker compose build

docker-up:
	docker compose up -d

docker-enter:
	docker compose exec nekoos bash

docker-down:
	docker compose down
