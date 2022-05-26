//
// Created by 屠昊天 on 2022/5/25.
//

#include "Heap.h"
#include "PageFrameAllocator.h"
#include "PageTable.h"
#include "../utils/assert.h" // NOLINT(modernize-deprecated-headers)

heap::Segment* lastSegment;
void* heapStart;
void* heapEnd;

void heap::init() {
    static_assert(PAGE_SIZE > sizeof(Segment));

    heapStart = (void*) HEAP_START_ADDRESS;
    void* newPage = GlobalPageFrameAllocator.requestHeapPage();
    assert(newPage == heapStart);
    heapEnd = (void*) ((size_t) heapStart + PAGE_SIZE);
    assert((uint64_t) heapEnd < MEMORY_END);

    auto segment = (Segment*) heapStart;
    segment->length = PAGE_SIZE - sizeof(Segment);
    segment->next = nullptr;
    segment->prev = nullptr;
    segment->free = true;
    lastSegment = segment;
}


void heap::expand(size_t length) {
    auto needPage = length / PAGE_SIZE;
    if (needPage * PAGE_SIZE != length) {
        needPage++;
    }

    auto segment = (Segment*) heapEnd;

    for (size_t i = 0; i < needPage; i++) {
        void* newPage = GlobalPageFrameAllocator.requestHeapPage();
        assert(newPage == heapEnd);
        heapEnd = (void*) ((size_t) heapEnd + PAGE_SIZE);
    }

    segment->free = true;
    segment->prev = lastSegment;
    segment->next = nullptr;
    segment->length = PAGE_SIZE * needPage - sizeof(Segment);
    lastSegment->next = segment;
    lastSegment = segment;

    segment->combinePrev();
}

void heap::Segment::combineNext() {
    if (this->next == nullptr) { return; }
    if (!this->next->free) { return; }

    if (this->next == lastSegment) {
        lastSegment = this;
    }

    if (next->next != nullptr) {
        next->next->prev = this;
    }
    this->length += this->next->length + sizeof(Segment);
    this->next = this->next->next;
}

void heap::Segment::combinePrev() {
    if (this->prev != nullptr && this->prev->free) {
        this->prev->combineNext();
    }
}

heap::Segment* heap::Segment::split(size_t splitLength) {
    if (this->length - splitLength < sizeof(Segment) + MIN_SEGMENT_SIZE) {
        return nullptr;
    }

    auto newSegment = (Segment*) ((size_t) this + splitLength + sizeof(Segment));
    newSegment->prev = this;
    newSegment->length = this->length - splitLength - sizeof(Segment);
    newSegment->free = this->free;
    this->length = splitLength;

    if (this->next != nullptr) {
        this->next->prev = newSegment;
        newSegment->next = this->next;
    }
    this->next = newSegment;

    if (lastSegment == this) { lastSegment = newSegment; }
    return newSegment;
}

void* malloc(size_t length) {
    assert(length != 0);
    size_t allocLength = 1;
    while (allocLength < length) {
        allocLength *= 2;
    }

    auto current = (heap::Segment*) heapStart;
    while (true) {
        assert(current != nullptr);
        if (current->free) {
            if (current->length > allocLength) {
                current->split(allocLength);
                current->free = false;
                return (void*) ((size_t) current + sizeof(heap::Segment));
            }
            if (current->length == allocLength) {
                current->free = false;
                return (void*) ((size_t) current + sizeof(heap::Segment));
            }
        }
        if (current->next == nullptr) { break; }
        current = current->next;
    }
    heap::expand(allocLength);
    return malloc(length);
}

void mfree(void* address) {
    heap::Segment* segment = (heap::Segment*) address - 1;
    segment->free = true;
    segment->combineNext();
    segment->combinePrev();
}
