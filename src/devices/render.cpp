//
// Created by 屠昊天 on 2022/5/19.
//

#include "render.h"

void Render::putChar(char chr) {
    char* p = (char*) (RENDER_MEMORY);
    *p = chr;
}

void Render::printString(const char* str) {
    char* p = (char*)str;
    while(*p != '\0'){
        Render::putChar(*p);
        p++;
    }
}
