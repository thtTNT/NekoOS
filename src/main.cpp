//
// Created by 屠昊天 on 2022/5/19.
//


#include "memory/PageFrameAllocator.h"
#include "memory/PageTable.h"
#include "utils/riscv.h"
#include "driver/render.h"
#include "driver/VirtioDisk.h"
#include "memory/Heap.h"

void initPageFrameAllocator() {
    GlobalPageFrameAllocator = PageFrameAllocator();
    Render::print("Page Frame Allocator initialized!\n");
}

void initPageTable() {
    auto page = GlobalPageFrameAllocator.requestPage();
    memset(page, 0, PAGE_SIZE);
    auto pageTable = (PageTable*) page;

    // map all physical memory
    for (uint64_t address = MEMORY_START; address < MEMORY_END; address += PAGE_SIZE) {
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

    KernelPageTable = pageTable;
    Render::print("Page Table initialized!\n");
}

extern char onTrap[];

void initTrap() {
    auto stvec = STVEC{};
    stvec.mode = 0;
    stvec.base = ((uint64_t) onTrap >> 2);
    w_stvec(stvec);

    Render::print("Trap initialized!\n");
}

void initDisk() {
    PrimaryDisk = VirtioDisk();
}

void initHeap() {
    heap::init();

    Render::print("Heap initialized!\n");
}

void main0() {
    int i = 1; // Hack GDB bug, not necessary
    Render::print("====================NekoOS v0.1====================\n");
    initTrap();
    initPageFrameAllocator();
    initPageTable();
    initHeap();
    mfree(malloc(8196));
    malloc(1024);
    malloc(1024);
    malloc(1024);
    malloc(1024);
    initDisk();
    Render::print("Total Memory: %llu Bytes\n", GlobalPageFrameAllocator.getTotalMemory());
    Render::print("Reserve Memory: %llu Bytes\n", GlobalPageFrameAllocator.getReserveMemory());
    Render::print("Locked Memory: %llu Bytes\n", GlobalPageFrameAllocator.getLockedMemory());
    while (true) {}
}