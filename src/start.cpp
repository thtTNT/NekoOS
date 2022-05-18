//
// Created by 屠昊天 on 2022/5/19.
//

__attribute__ ((aligned (16))) char stack0[4096];

extern "C" void _start() {
    char* buffer = (char*) 0x10000000;
    *buffer = 'h';
    while (true) {}
}