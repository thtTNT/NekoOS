//
// Created by 屠昊天 on 2022/5/25.
//


#pragma once

#include <cstddef>
#include "Memory.h"
#include "../utils/List.h"

#define MIN_SEGMENT_SIZE 0b1000

namespace heap {
    struct Segment {
        size_t length;
        Segment* next;
        Segment* prev;
        bool free;

        void combineNext();

        void combinePrev();

        Segment* split(size_t length);
    }__attribute__((aligned(MIN_SEGMENT_SIZE)));

    void init();

    void expand(size_t length);

}

void* malloc(size_t length);

void mfree(void* address);

