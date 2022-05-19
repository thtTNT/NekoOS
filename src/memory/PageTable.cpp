//
// Created by 屠昊天 on 2022/5/20.
//

#include "PageTable.h"
#include "PageFrameAllocator.h"

void PageTable::mapMemory(void* virtualAddress, void* physicalAddress) {
    mapMemory(*(VirtualAddress*) &virtualAddress, *(PhysicalAddress*) &physicalAddress);
}

void PageTable::mapMemory(VirtualAddress virtualAddress, PhysicalAddress physicalAddress) {
    auto pte = this->root.entries[virtualAddress.l2];
    if (!pte.valid) {
        auto page = GlobalPageFrameAllocator.requestPage();
        memset(page, 0, PAGE_SIZE);
        this->root.entries[virtualAddress.l2].physicalPageNumber = (((uint64_t) (page) / PAGE_SIZE) & PPN_SHADOW);
        this->root.entries[virtualAddress.l2].valid = true;
    }
    auto pd = (PageDirectory*) (uint64_t) (this->root.entries[virtualAddress.l2].physicalPageNumber * PAGE_SIZE);

    pte = pd->entries[virtualAddress.l1];
    if (!pte.valid) {
        auto page = GlobalPageFrameAllocator.requestPage();
        memset(page, 0, PAGE_SIZE);
        pd->entries[virtualAddress.l1].physicalPageNumber = (((uint64_t) (page) / PAGE_SIZE) & PPN_SHADOW);
        pd->entries[virtualAddress.l1].valid = true;
    }
    pd = (PageDirectory*) (uint64_t) (pd->entries[virtualAddress.l1].physicalPageNumber * PAGE_SIZE);

    pte = pd->entries[virtualAddress.l0];
    if (!pte.valid) {
        pd->entries[virtualAddress.l0].physicalPageNumber = physicalAddress.physicalPageNumber;
        pd->entries[virtualAddress.l0].valid = true;
        pd->entries[virtualAddress.l0].readable = true;
        pd->entries[virtualAddress.l0].writable = true;
        pd->entries[virtualAddress.l0].executable = true;
    }
}