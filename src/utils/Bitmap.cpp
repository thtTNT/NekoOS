//
// Created by 屠昊天 on 2022/5/19.
//

#include "Bitmap.h"

bool Bitmap::operator[](uint64_t index) {
    return this->get(index);
}

bool Bitmap::get(uint64_t index) {
    uint64_t block = index / 8;
    uint8_t offset = index % 8;
    uint8_t blockData = *(this->map + block);
    return (blockData >> offset) & 1;
}

void Bitmap::set(uint64_t index, bool value) {
    uint64_t block = index / 8;
    uint8_t offset = index % 8;
    uint8_t blockData = *(this->map + block);

    blockData &= ~(0b10000000 >> offset);
    if (value) {
        blockData |= 0b10000000 >> offset;
    }
    this->map[block] = blockData;
}




