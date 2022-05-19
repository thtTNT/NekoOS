//
// Created by 屠昊天 on 2022/5/19.
//


#pragma once
#define KERNEL_START 0x80000000
#define KERNEL_END ((uint64_t)KERNEL_END_ADDRESS)
#define MEMORY_SIZE 128 * 1024 * 1024
#define PAGE_SIZE 4096

#include <cstddef>
#include <cstdint>

extern char KERNEL_END_ADDRESS[];

void memset(void* start, uint8_t value, size_t length);