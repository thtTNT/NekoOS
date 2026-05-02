//
// Created by 屠昊天 on 2022/5/30.
//

#pragma once

#include <stdint.h>

#define TIMER_INTERVAL 100000  // ~10ms at 10MHz

namespace clint {
    static inline uint64_t readMtime() {
        uint64_t time;
        asm volatile("csrr %0, time" : "=r"(time));
        return time;
    }

    static inline void setNextTimer() {
        uint64_t next = readMtime() + TIMER_INTERVAL;
        // SBI TIME extension: a7 = 0x54494D45, a6 = 0 (set_timer), a0 = stime_value
        register uint64_t a7 asm("a7") = 0x54494D45;
        register uint64_t a6 asm("a6") = 0;
        register uint64_t a0 asm("a0") = next;
        asm volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a6));
    }
}
