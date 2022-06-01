//
// Created by 屠昊天 on 2022/5/19.
//

#include "Memory.h"
#include "PageFrameAllocator.h"

void memset(void* start, uint8_t value, size_t length) {
    auto p = (uint8_t*) start;
    for (size_t i = 0; i < length; i++) {
        *p = value;
    }
}

void* memcpy(void* dest, const void* src, size_t length) {
    char* srcPointer = (char*) src;
    char* destPointer = (char*) dest;
    for (size_t i = 0; i < length; i++) {
        destPointer[i] = srcPointer[i];
    }
    return dest;
}
