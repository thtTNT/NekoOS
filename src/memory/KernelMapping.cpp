#include "KernelMapping.h"
#include "PageFrameAllocator.h"
#include "Memory.h"
#include "../utils/riscv.h"
#include "../driver/VirtioDisk.h"
#include <string.h>

static void mapKernelSpace(PageTable* pageTable) {
    for (uint64_t address = KERNEL_START; address < MEMORY_END; address += PAGE_SIZE) {
        pageTable->mapMemory((void*) address, (void*) address);
    }

    pageTable->mapMemory((void*) 0x10000000, (void*) 0x10000000);          // UART0
    pageTable->mapMemory((void*) VIRTIO_ADDRESS, (void*) VIRTIO_ADDRESS);   // VirtIO
    pageTable->mapMemory((void*) PLIC, (void*) PLIC, PLIC_SIZE / PAGE_SIZE); // PLIC
}

PageTable* initKernelPageTable() {
    auto page = GlobalPageFrameAllocator.requestPage();
    memset(page, 0, PAGE_SIZE);
    auto pageTable = (PageTable*) page;

    mapKernelSpace(pageTable);

    SATP satp{};
    satp.physicalPageNumber = (uint64_t) page / PAGE_SIZE;
    satp.mode = 0x8;
    w_satp(satp);
    sfence_vma();

    KernelPageTable = pageTable;
    return pageTable;
}

PageTable* createUserPageTable() {
    auto page = GlobalPageFrameAllocator.requestPage();
    memset(page, 0, PAGE_SIZE);
    auto pageTable = (PageTable*) page;

    mapKernelSpace(pageTable);

    return pageTable;
}
