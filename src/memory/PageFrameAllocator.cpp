//
// Created by 屠昊天 on 2022/5/19.
//

#include "PageFrameAllocator.h"
#include "../utils/assert.h"

PageFrameAllocator GlobalPageFrameAllocator; // NOLINT(cert-err58-cpp)


size_t addressToPageIndex(uint64_t address) {
    return (address - KERNEL_START) / PAGE_SIZE;
}

PageFrameAllocator::PageFrameAllocator() {
    this->heap = addressToPageIndex(PAGE_BITMAP_END);
    this->totalMemory = MEMORY_SIZE;
    this->reserveMemory = 0;
    this->lockedMemory = 0;

    // init bitmap
    memset((void*) PAGE_BITMAP_ADDRESS, 0, PAGE_BITMAP_SIZE);
    this->bitmap = Bitmap{MEMORY_SIZE / PAGE_SIZE / 8, (uint8_t*) PAGE_BITMAP_ADDRESS};

    //reserve kernel and bitmap memory
    for (size_t index = addressToPageIndex(KERNEL_START); index < addressToPageIndex(KERNEL_END); index++) {
        this->reservePage(index);
    }
    for (size_t index = addressToPageIndex(PAGE_BITMAP_ADDRESS); index < addressToPageIndex(PAGE_BITMAP_END); index++) {
        this->reservePage(index);
    }
}

void PageFrameAllocator::reservePage(size_t index) {
    this->reserveMemory += PAGE_SIZE;
    this->bitmap.set(index, true);
}

void PageFrameAllocator::lockPage(size_t index) {
    this->lockedMemory += PAGE_SIZE;
    this->bitmap.set(index, true);
}

void PageFrameAllocator::unreservePage(size_t index) {
    this->reserveMemory -= PAGE_SIZE;
    this->bitmap.set(index, false);
}

void PageFrameAllocator::unlockPage(size_t index) {
    this->lockedMemory -= PAGE_SIZE;
    this->bitmap.set(index, false);
}

size_t PageFrameAllocator::getTotalMemory() const {
    return totalMemory;
}

size_t PageFrameAllocator::getReserveMemory() const {
    return reserveMemory;
}

size_t PageFrameAllocator::getLockedMemory() const {
    return lockedMemory;
}

void* PageFrameAllocator::requestPage() {
    for (size_t index = PAGE_COUNT - 1; index > heap; index--) {
        if (!this->bitmap[index]) {
            this->lockPage(index);
            return (void*) (uint64_t) (KERNEL_START + (index * PAGE_SIZE));
        }
    }
    return nullptr;
}

void* PageFrameAllocator::requestPages(size_t amount) {
    uint8_t count = 0;
    for (size_t index = PAGE_COUNT - 1; index > heap; index--) {
        if (!this->bitmap[index]) {
            if (count == amount - 1) {
                for (size_t i = index; i <= index + count; i++) {
                    assert(!this->bitmap[i]);
                    this->lockPage(i);
                }
                return (void*) (uint64_t) (KERNEL_START + (index * PAGE_SIZE));
            } else {
                count++;
            }
        } else {
            count = 0;
        }
    }
    return nullptr;
}

void* PageFrameAllocator::requestHeapPage() {
    for (int index = HEAP_START_PAGE; index < PAGE_COUNT; index++) {
        if (!this->bitmap[index]) {
            this->reservePage(index);
            heap = index;
            return (void*) (uint64_t) (KERNEL_START + (index * PAGE_SIZE));
        }
    }
    return nullptr;
}

