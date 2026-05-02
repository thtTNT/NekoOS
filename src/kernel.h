//
// Created by 屠昊天 on 2022/5/30.
//


#pragma once

#include "./utils/riscv.h"
#include <stdint.h>

static inline uint64_t cpuid() {
    return r_tp();
}