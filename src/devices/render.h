//
// Created by 屠昊天 on 2022/5/19.
//


#pragma once
#define RENDER_MEMORY 0x10000000

class Render{
public:
    static void putChar(char chr);
    static void printString(const char* str);
};
