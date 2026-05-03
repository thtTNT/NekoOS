#pragma once

#include "Process.h"
#include "../utils/ListNode.h"

extern Process* current;
extern Process* idleProcess;
extern ProcessContext* currentCtx;
extern ListNode readyQueue;

void initScheduler();
void schedule();
Process* createKernelProcess(void (*entry)());
Process* createUserProcess(void* entry, void* ustackTop);

extern "C" void switch_to(ProcessContext* nextCtx);
