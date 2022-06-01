//
// Created by 屠昊天 on 2022/5/28.
//

#include "plic.h"
#include "../memory/MemoryLayout.h"

#include "../kernel.h"

static inline uint32_t &getPlicSenableRef(uint64_t hart) {
    return *(uint32_t*) (PLIC + 0x2080 + hart * 0x100);
}

static inline uint32_t &getPlicSpriorityRef(uint64_t hart) {
    return *(uint32_t*) (PLIC + 0x201000 + hart * 0x2000);
}

void plic::init() {
    *(uint32_t*) (PLIC + UART0_IRQ * 4) = 1;
    *(uint32_t*) (PLIC + VIRTIO0_IRQ * 4) = 1;
}

uint32_t plic::getClaim() {
    uint64_t hart = cpuid();

    uint32_t irq = *(uint32_t*) (PLIC + 0x201004 + hart * 0x2000);
    return irq;
}

void plic::initHart() {
    uint64_t hart = cpuid();

    getPlicSenableRef(hart) = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

    getPlicSpriorityRef(hart) = 0;
}
