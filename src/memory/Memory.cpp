//
// Created by 屠昊天 on 2022/5/19.
//

#include "Memory.h"
#include "PageFrameAllocator.h"

void memset(void* start, uint8_t value, size_t length) {
    auto p = (uint8_t*) start;
    for (int i = 0; i < length; i++) {
        *p = value;
    }
}
