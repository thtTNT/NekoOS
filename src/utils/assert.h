//
// Created by 屠昊天 on 2022/5/26.
//

#pragma once

#include "../driver/render.h"

#ifdef NDEBUG
#define assert(expr) (void(0))
#else
#define assert(expr) ((expr) ? void(0) : __assert_fail(#expr,__FILE__,__LINE__))
#endif


void __assert_fail(const char* __assertion, const char* __file, unsigned int __line);
