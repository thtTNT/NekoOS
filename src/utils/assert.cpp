//
// Created by 屠昊天 on 2022/5/27.
//
#include "assert.h"


void __assert_fail(const char* __assertion, const char* __file, unsigned int __line) {
    Render::print("ASSERT FAIL IN: %s , %s:%llu", __assertion, __file, __line);
    while (true) {}
}
