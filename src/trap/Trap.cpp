//
// Created by 屠昊天 on 2022/5/20.
//

#include "Trap.h"
#include "../process/Scheduler.h"
#include "../driver/render.h"
#include "../driver/VirtioDisk.h"
#include "../kernel.h"
#include "../utils/riscv.h"
#include "../syscall/syscall.h"
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
        clint::setNextTimer();
        schedule();
    } else if (cause == SCAUSE_ECALL_USER) {
        current->context.sepc += 4;
        handleEcall();
    } else if (cause.code == 12 || cause.code == 13 || cause.code == 15) {
        uint64_t stval, sepc;
        asm volatile("csrr %0, stval" : "=r"(stval));
        asm volatile("csrr %0, sepc" : "=r"(sepc));
        Render::print("Page fault: scause=%lu stval=0x%lx sepc=0x%lx\n", cause.code, stval, sepc);
        while (true) {}
    } else {
        Render::print("Trap: unknown scause=0x%lx\n", *(uint64_t*)&cause);
        while (true) {}
    }
}
