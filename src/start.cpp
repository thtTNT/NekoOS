//
// Created by 屠昊天 on 2022/5/19.
//

__attribute__ ((aligned (16))) char stack0[4096];

#include "devices/render.h"

extern "C" void kernel() {
    int i = 1; // Hack GDB bug, now necessary
    Render::printString("Hello World!!!");
    while (true) {}
}