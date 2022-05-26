//
// Created by 屠昊天 on 2022/5/19.
//


#pragma once
#include "MemoryLayout.h"

#include <cstddef>
#include <cstdint>

extern char KERNEL_END_ADDRESS[];

void memset(void* start, uint8_t value, size_t length);


struct VirtualAddress {
    uint16_t offset: 12;
    uint16_t l0: 9;
    uint16_t l1: 9;
    uint16_t l2: 9;
    uint32_t ext: 25;
}__attribute__((packed));

struct PhysicalAddress {
    uint16_t offset: 12;
    uint64_t physicalPageNumber: 44;
    uint16_t reserve: 8;
}__attribute__((packed));
