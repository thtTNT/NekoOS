//
// Created by 屠昊天 on 2022/5/20.
//

#include "Trap.h"
#include "../driver/render.h"
#include "../driver/VirtioDisk.h"
#include "../kernel.h"
#include "../utils/riscv.h"
#include "plic.h"
#include "clint.h"

void onDeviceInterrupt() {
    uint32_t irq = plic::getClaim();

    switch (irq) {
        case VIRTIO0_IRQ:
            PrimaryDisk.onInterrupt();
            break;
        default:
            Render::print("Unexpected interrupt irq=%lu\n", irq);
    }
}

void onTrap() {
    SCAUSE cause = r_scause();
    if (cause == SCAUSE_INTERRUPT_SUPERVISOR_EXTERNAL_INTERRUPT) {
        onDeviceInterrupt();
    } else if (cause == SCAUSE_INTERRUPT_SUPERVISOR_TIMER_INTERRUPT) {
        Render::print("Timer interrupt\n");
        clint::setNextTimer();
    } else {
        Render::print("Trap: unknown trap\n");
        while (true) {}
    }
}


