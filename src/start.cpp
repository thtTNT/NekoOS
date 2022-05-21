//
// Created by 屠昊天 on 2022/5/21.
//

#include "utils/riscv.h"

void main0();

__attribute__ ((aligned (16))) char stack0[4096];

extern "C" void start() {
    MSTATUS mstatus = r_mstatus();
    mstatus.mpp = SUPERVISOR;
    w_mstatus(mstatus);

    w_mepc((void*) main0);

    w_satp(SATP{});

    // delegate all M-mode interrupt and exception to S-mode
    w_medeleg(0xffff);
    w_mideleg(0xffff);

    // enable all interrupt and exception in S-mode
    SIE sie = r_sie();
    sie.seie = true;
    sie.stie = true;
    sie.ssie = true;
    w_sie(sie);

    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    asm volatile("mret");
}