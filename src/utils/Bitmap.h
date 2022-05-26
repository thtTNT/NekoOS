//
// Created by 屠昊天 on 2022/5/19.
//


#pragma once

#include <cstddef>
#include <cstdint>

struct Bitmap {
    size_t size;
    uint8_t* map;

    bool operator[](uint64_t index);

    bool get(uint64_t index);

    void set(uint64_t index, bool value);
};

template<size_t size>
struct BitArray {
    uint8_t data[size / 8];

    bool operator[](uint64_t index);

    bool get(uint64_t index);

    void set(uint64_t index, bool value);
};

