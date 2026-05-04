#include "Scheduler.h"
#include "../utils/riscv.h"
#include "../memory/PageTable.h"
#include "../memory/Heap.h"
#include "../memory/Memory.h"
#include "../memory/KernelMapping.h"
#include "../memory/PageFrameAllocator.h"

Process* current = nullptr;
Process* idleProcess = nullptr;
ProcessContext* currentCtx = nullptr;
ListNode readyQueue;
void initScheduler() {
    list_init(&readyQueue);
}

void schedule() {
    if (current != nullptr && current->state == RUNNING && current != idleProcess) {
        current->state = READY;
        list_pushBack(&readyQueue, &current->node);
    }

    Process* next;

    if (list_empty(&readyQueue)) {
        if (current == idleProcess) {
            return;
        }
        next = idleProcess;
    } else {
        ListNode* node = readyQueue.next;
        list_remove(node);
        next = container_of(node, Process, node);
    }

    next->state = RUNNING;

    if (next != current) {
        SATP satp{};
        satp.physicalPageNumber = (uint64_t)next->pageTable / PAGE_SIZE;
        satp.mode = 0x8;
        w_satp(satp);
        sfence_vma();

        current = next;
        currentCtx = &next->context;

        uint64_t trapSP = (next->pageTable == KernelPageTable) ? 0 : (uint64_t)next->kernelStack;
        switch_to(currentCtx, trapSP);
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

static const int USER_STACK_PAGES = 16;

Process* createUserProcess(void* entry, void* ustackTop) {
    Process* proc = (Process*)malloc(sizeof(Process));
    if (proc == nullptr) return nullptr;

    void* kstack = malloc(PAGE_SIZE);
    if (kstack == nullptr) {
        mfree(proc);
        return nullptr;
    }

    PageTable* pageTable = createUserPageTable();

    uint64_t stackBottom = (uint64_t)ustackTop - USER_STACK_PAGES * PAGE_SIZE;
    for (int i = 0; i < USER_STACK_PAGES; i++) {
        void* va = (void*)(stackBottom + i * PAGE_SIZE);
        void* pa = GlobalPageFrameAllocator.requestPage();
        memset(pa, 0, PAGE_SIZE);
        pageTable->mapMemory(va, pa, 1, true);
    }

    // Alias: same physical page at a different VA with U=1 for user execution.
    // S-mode retains the original identity mapping at U=0 for kernel access.
    uint64_t codePage = (uint64_t)entry & ~(PAGE_SIZE - 1);
    uint64_t offset = (uint64_t)entry - codePage;
    uint64_t userCodeAddr = 0x1000;
    pageTable->mapMemory((void*)userCodeAddr, (void*)codePage, 1, true);

    proc->pid = nextPid++;
    proc->state = READY;
    proc->pageTable = pageTable;
    proc->kernelStack = (void*)((uint64_t)kstack + PAGE_SIZE);
    list_init(&proc->node);

    memset(&proc->context, 0, sizeof(ProcessContext));
    proc->context.regs[2] = (uint64_t)ustackTop;
    proc->context.sepc = userCodeAddr + offset;

    SSTATUS sstatus{};
    sstatus.spp = 0;
    sstatus.spie = 1;
    proc->context.sstatus = *(uint64_t*)&sstatus;

    return proc;
}
