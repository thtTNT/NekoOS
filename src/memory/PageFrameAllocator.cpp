//
// Created by 屠昊天 on 2022/5/19.
//

#include "PageFrameAllocator.h"

PageFrameAllocator GlobalPageFrameAllocator; // NOLINT(cert-err58-cpp)


size_t addressToPageIndex(uint64_t address) {
    return (address - KERNEL_START) / PAGE_SIZE;
}

PageFrameAllocator::PageFrameAllocator() {
    this->totalMemory = MEMORY_SIZE;
    this->reserveMemory = 0;
    this->lockedMemory = 0;

    // init bitmap
    int requestPage = MEMORY_SIZE / PAGE_SIZE / 8 / PAGE_SIZE;
    int startPage = (int) (KERNEL_END - KERNEL_START) / PAGE_SIZE + 1;
    void* mapAddress = (void*) (uint64_t) (KERNEL_START + startPage * PAGE_SIZE);
    memset(mapAddress, 0, requestPage * PAGE_SIZE);
    this->bitmap = Bitmap{MEMORY_SIZE / PAGE_SIZE / 8, (uint8_t*) mapAddress};

    //reserve kernel and bitmap memory
    for (size_t index = addressToPageIndex(KERNEL_START); index <= addressToPageIndex(KERNEL_END); index++) {
        this->reservePage(index);
    }
    for (size_t index = startPage; index < startPage + requestPage; index++) {
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
    for (int index = 0; index < PAGE_COUNT; index++) {
        if (!this->bitmap[index]) {
            this->lockPage(index);
            return (void*) (uint64_t) (KERNEL_START + (index * PAGE_SIZE));
        }
    }
    return nullptr;
}
