//
// Created by 屠昊天 on 2022/5/20.
//


#pragma once
#define PPN_SHADOW 0xfffffffffff

#include "Memory.h"


struct PageTableEntry {
    bool valid: 1;
    bool readable: 1;
    bool writable: 1;
    bool executable: 1;
    bool user: 1;
    bool global: 1;
    bool accessed: 1;
    bool dirty: 1;
    uint16_t reserved1: 2;
    uint64_t physicalPageNumber: 44;
    uint16_t reserved: 10;
};

struct PageDirectory {
    PageTableEntry entries[512];
}__attribute__((aligned(0x1000)));

struct PageTable {
    PageDirectory root;

    void mapMemory(void* virtualAddress, void* physicalAddress);

    void mapMemory(VirtualAddress virtualAddress, PhysicalAddress physicalAddress);

}__attribute__((aligned(0x1000)));