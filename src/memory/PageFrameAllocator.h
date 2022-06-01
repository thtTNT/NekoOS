//
// Created by 屠昊天 on 2022/5/19.
//


#pragma once

#include "../utils/Bitmap.h"
#include "Memory.h"


class PageFrameAllocator {
private:
    Bitmap bitmap{};
    size_t totalMemory;
    size_t reserveMemory;
    size_t lockedMemory;
    size_t heap;
public:
    PageFrameAllocator();

    void reservePage(size_t index);

    void lockPage(size_t index);

    void unreservePage(size_t index);

    void unlockPage(size_t index);

    void* requestPage();

    void* requestHeapPage();

    [[nodiscard]] size_t getTotalMemory() const;

    [[nodiscard]] size_t getReserveMemory() const;

    [[nodiscard]] size_t getLockedMemory() const;

    void* requestPages(size_t amount);
};

extern PageFrameAllocator GlobalPageFrameAllocator;