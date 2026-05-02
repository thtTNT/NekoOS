#include "Scheduler.h"
#include "../utils/riscv.h"
#include "../memory/PageTable.h"
#include "../memory/Heap.h"
#include "../memory/Memory.h"

Process* current = nullptr;
ProcessContext* currentCtx = nullptr;
ListNode readyQueue;

void initScheduler() {
    list_init(&readyQueue);
}

void schedule() {
    if (current != nullptr && current->state == RUNNING) {
        current->state = READY;
        list_pushBack(&readyQueue, &current->node);
    }

    if (list_empty(&readyQueue)) {
        return;
    }

    ListNode* node = readyQueue.next;
    list_remove(node);
    Process* next = container_of(node, Process, node);
    next->state = RUNNING;

    if (next != current) {
        current = next;
        currentCtx = &next->context;
        switch_to(currentCtx);
    }
}

static uint64_t nextPid = 0;

Process* createKernelProcess(void (*entry)()) {
    Process* proc = (Process*)malloc(sizeof(Process));
    if (proc == nullptr) return nullptr;

    void* kstack = malloc(PAGE_SIZE);
    if (kstack == nullptr) {
        mfree(proc);
        return nullptr;
    }

    proc->pid = nextPid++;
    proc->state = READY;
    proc->pageTable = KernelPageTable;
    proc->kernelStack = (void*)((uint64_t)kstack + PAGE_SIZE);
    list_init(&proc->node);

    memset(&proc->context, 0, sizeof(ProcessContext));
    proc->context.regs[2] = (uint64_t)proc->kernelStack;
    proc->context.sepc = (uint64_t)entry;

    SSTATUS sstatus{};
    sstatus.spp = 1;
    sstatus.spie = 1;
    proc->context.sstatus = *(uint64_t*)&sstatus;

    return proc;
}
