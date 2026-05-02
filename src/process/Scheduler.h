#pragma once

#include "Process.h"
#include "../utils/ListNode.h"

extern Process* current;
extern ProcessContext* currentCtx;
extern ListNode readyQueue;

void initScheduler();
void schedule();
Process* createKernelProcess(void (*entry)());

extern "C" void switch_to(ProcessContext* nextCtx);
