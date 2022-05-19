//
// Created by 屠昊天 on 2022/5/19.
//

__attribute__ ((aligned (16))) char stack0[4096];

#include "devices/render.h"
#include "memory/PageFrameAllocator.h"

extern "C" void kernel() {
    int i = 1; // Hack GDB bug, now necessary
    GlobalPageFrameAllocator = PageFrameAllocator();
    Render::print("Total Memory: %lu Bytes\n", GlobalPageFrameAllocator.getTotalMemory());
    Render::print("Reserve Memory: %lu Bytes\n", GlobalPageFrameAllocator.getReserveMemory());
    Render::print("Locked Memory: %lu Bytes\n", GlobalPageFrameAllocator.getLockedMemory());

    while (true) {}
}