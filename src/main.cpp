//
// Created by 屠昊天 on 2022/5/19.
//


#include "memory/PageFrameAllocator.h"
#include "memory/KernelMapping.h"
#include "memory/Heap.h"
#include "memory/Memory.h"
#include "utils/riscv.h"
#include "driver/render.h"
#include "driver/VirtioDisk.h"
#include "trap/plic.h"
#include "process/Scheduler.h"


void initPageFrameAllocator() {
    GlobalPageFrameAllocator = PageFrameAllocator();
    Render::print("Page Frame Allocator initialized!\n");
}

void initPageTable() {
    initKernelPageTable();
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

void idleEntry() {
    while (true) {
        wfi();
    }
}

void userTestEntry() {
    while (true) {
        asm volatile("nop");
    }
}

void main0() {
    Render::print("====================NekoOS v0.1====================\n");
    initPageFrameAllocator();
    initPageTable();
    initHeap();
    initTrap();
    initDisk();
    initScheduler();

    Process* idle = createKernelProcess(idleEntry);
    idle->pid = 0;
    idleProcess = idle;

    Process* userTest = createUserProcess((void*)userTestEntry, (void*)0x3FFFFFF000);
    list_pushBack(&readyQueue, &userTest->node);

    __sync_synchronize();
    Render::print("Total Memory: %llu Bytes\n", GlobalPageFrameAllocator.getTotalMemory());
    Render::print("Reserve Memory: %llu Bytes\n", GlobalPageFrameAllocator.getReserveMemory());
    Render::print("Locked Memory: %llu Bytes\n", GlobalPageFrameAllocator.getLockedMemory());
    setInterrupt(true);
    schedule();
}