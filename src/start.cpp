//
// Created by 屠昊天 on 2022/5/21.
//

#include "utils/riscv.h"
#include "trap/clint.h"

void main0();

__attribute__ ((aligned (16))) char stack0[4096];

extern "C" void start() {
    // OpenSBI has already configured medeleg, mideleg, and PMP.
    // We arrive in S-mode with interrupts disabled.

    SIE sie = r_sie();
    sie.seie = true;
    sie.stie = true;
    sie.ssie = true;
    w_sie(sie);

    // Kick off the first timer interrupt
    clint::setNextTimer();

    main0();
}
