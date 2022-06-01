//
// Created by 屠昊天 on 2022/5/19.
//


#include "memory/PageFrameAllocator.h"
#include "memory/PageTable.h"
#include "memory/Heap.h"
#include "memory/Memory.h"
#include "utils/riscv.h"
#include "driver/render.h"
#include "driver/VirtioDisk.h"
#include "trap/plic.h"


void initPageFrameAllocator() {
    GlobalPageFrameAllocator = PageFrameAllocator();
    Render::print("Page Frame Allocator initialized!\n");
}

void initPageTable() {
    auto page = GlobalPageFrameAllocator.requestPage();
    memset(page, 0, PAGE_SIZE);
    auto pageTable = (PageTable*) page;

    // map all physical memory
    for (uint64_t address = KERNEL_START; address < MEMORY_END; address += PAGE_SIZE) {
        pageTable->mapMemory((void*) address, (void*) address);
    }

    // map uart0
    pageTable->mapMemory((void*) 0x10000000, (void*) 0x10000000);

    // map virtio disk
    pageTable->mapMemory((void*) VIRTIO_ADDRESS, (void*) VIRTIO_ADDRESS);

    // map PLIC
    static_assert(PLIC_SIZE % PAGE_SIZE == 0);
    pageTable->mapMemory((void*) PLIC, (void*) PLIC, PLIC_SIZE / PAGE_SIZE);


    SATP satp = SATP{};
    satp.physicalPageNumber = (uint64_t) page / PAGE_SIZE;
    satp.mode = 0x8;
    w_satp(satp);
    sfence_vma();

    KernelPageTable = pageTable;
    Render::print("Page Table initialized!\n");
}

extern char trap_entry[];

void initTrap() {
    auto stvec = STVEC{};
    stvec.mode = 0;
    stvec.base = ((uint64_t) trap_entry >> 2);
    w_stvec(stvec);

    plic::init();
    plic::initHart();

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
    Render::print("====================NekoOS v0.1====================\n");
    initPageFrameAllocator();
    initPageTable();
    initHeap();
    initTrap();
    initDisk();
    __sync_synchronize();
    Render::print("Total Memory: %llu Bytes\n", GlobalPageFrameAllocator.getTotalMemory());
    Render::print("Reserve Memory: %llu Bytes\n", GlobalPageFrameAllocator.getReserveMemory());
    Render::print("Locked Memory: %llu Bytes\n", GlobalPageFrameAllocator.getLockedMemory());
    setInterrupt(true);
    DiskOperationRequest request{};
    request.length = 1024;
    void* address = malloc(1024);
    request.address = address;
    request.type = DiskOperationType::READ;
    request.sector = 1;
    request.valid = false;
    PrimaryDisk.operate(request);
    while (true) {}
}