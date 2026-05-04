#include "syscall.h"
#include "../process/Scheduler.h"
#include "../driver/render.h"

void handleEcall() {
    uint64_t nr = current->context.regs[17];  // a7: syscall number
    uint64_t a0 = current->context.regs[10];  // a0: arg0 / return value
    uint64_t a1 = current->context.regs[11];  // a1: arg1
    uint64_t a2 = current->context.regs[12];  // a2: arg2
    uint64_t ret = 0;

    switch (nr) {
    case SYS_YIELD:
        schedule();
        break;
    case SYS_WRITE:
        if (a0 == FD_STDOUT || a0 == FD_STDERR) {
            for (uint64_t i = 0; i < a2; i++)
                Render::putChar(((const char*)a1)[i]);
            ret = a2;
        }
        break;
    case SYS_EXIT:
        current->state = DEAD;
        schedule();   // never returns
        while (1) {}
    default:
        break;
    }
    current->context.regs[10] = ret;
}
