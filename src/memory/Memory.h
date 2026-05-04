//
// Created by 屠昊天 on 2022/5/19.
//


#pragma once

#include "MemoryLayout.h"

#include <stddef.h>
#include <stdint.h>

extern char KERNEL_END_ADDRESS[];

struct VirtualAddress {
    uint16_t offset: 12;
    uint16_t l0: 9;
    uint16_t l1: 9;
    uint16_t l2: 9;
    uint32_t ext: 25;

    VirtualAddress operator+(uint64_t value) {
        auto address = *(uint64_t*) this + value;
        return *(VirtualAddress*) &address;
    }
}__attribute__((packed));

struct PhysicalAddress {
    uint16_t offset: 12;
    uint64_t physicalPageNumber: 44;
    uint16_t reserve: 8;

    PhysicalAddress operator+(uint64_t value) {
        auto address = *(uint64_t*) this + value;
        return *(PhysicalAddress*) &address;
    }
}__attribute__((packed));
