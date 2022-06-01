//
// Created by 屠昊天 on 2022/5/28.
//


#pragma once

#include <cstdint>

#define VIRTIO0_IRQ 1
#define UART0_IRQ 10

namespace plic {
    void init();

    void initHart();

    uint32_t getClaim();
}