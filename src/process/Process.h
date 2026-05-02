//
// Created by 屠昊天 on 2026/5/2.
//


#pragma once

#include <stdint.h>
#include "../memory/PageTable.h"
#include "../utils/ListNode.h"

struct ProcessContext {
    uint64_t regs[32];  // x0-x31
    uint64_t sepc;      // 程序计数器
    uint64_t sstatus;   // 进程状态寄存器
};

enum ProcessState { READY, RUNNING, BLOCKED, DEAD };

struct Process {
    uint64_t pid;
    ProcessState state;
    ProcessContext context;
    PageTable* pageTable;  // 进程页表
    void* kernelStack;     // 内核栈（用于 trap 处理）
    ListNode node;         // 调度队列结点
};
