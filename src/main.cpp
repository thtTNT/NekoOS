//
// Created by 屠昊天 on 2022/5/19.
//


#include "memory/PageFrameAllocator.h"
#include "memory/PageTable.h"
#include "utils/riscv.h"
#include "driver/render.h"
#include "driver/VirtioDisk.h"

void initPageFrameAllocator() {
    GlobalPageFrameAllocator = PageFrameAllocator();
}

void initPageTable() {
    auto page = GlobalPageFrameAllocator.requestPage();
    memset(page, 0, PAGE_SIZE);
    auto pageTable = (PageTable*) page;

    // map kernel code
    for (uint64_t address = KERNEL_START; address < KERNEL_END; address += PAGE_SIZE) {
        pageTable->mapMemory((void*) address, (void*) address);
    }

    // map page frame allocator bit map
    for (uint64_t address = (KERNEL_END / PAGE_SIZE + 1) * PAGE_SIZE;
         address < (KERNEL_END / PAGE_SIZE + 1) * PAGE_SIZE + MEMORY_SIZE / PAGE_SIZE / 8; address += PAGE_SIZE) {
        pageTable->mapMemory((void*) address, (void*) address);
    }

    // map uart0
    pageTable->mapMemory((void*) 0x10000000, (void*) 0x10000000);

    // map virtio disk
    pageTable->mapMemory((void*) VIRTIO_ADDRESS, (void*) VIRTIO_ADDRESS);


    SATP satp = SATP{};
    satp.physicalPageNumber = (uint64_t) page / PAGE_SIZE;
    satp.mode = 0x8;
    w_satp(satp);
    sfence_vma();
}

extern char onTrap[];

void initTrap() {
    auto stvec = STVEC{};
    stvec.mode = 0;
    stvec.base = ((uint64_t) onTrap >> 2);
    w_stvec(stvec);
}

void initDisk() {
    PrimaryDisk = VirtioDisk();
}

void main0() {
    int i = 1; // Hack GDB bug, not necessary
    Render::print("====================NekoOS v0.1====================\n");
    initPageFrameAllocator();
    initPageTable();
    initTrap();
    initDisk();
    Render::print("Total Memory: %lu Bytes\n", GlobalPageFrameAllocator.getTotalMemory());
    Render::print("Reserve Memory: %lu Bytes\n", GlobalPageFrameAllocator.getReserveMemory());
    Render::print("Locked Memory: %lu Bytes\n", GlobalPageFrameAllocator.getLockedMemory());
    while (true) {}
}