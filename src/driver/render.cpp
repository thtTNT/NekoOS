//
// Created by 屠昊天 on 2022/5/19.
//

#include "render.h"
#include <cstdint>

void Render::putChar(char chr) {
    char* p = (char*) (RENDER_MEMORY);
    *p = chr;
}

void Render::printString(const char* str) {
    char* p = (char*) str;
    while (*p != '\0') {
        Render::putChar(*p);
        p++;
    }
}

void printUint64(uint64_t number) {
    if (number == 0) {
        Render::putChar('0');
        return;
    }
    char buffer[21];
    int size = 0;
    while (number != 0) {
        buffer[size] = number % 10 + '0';
        number /= 10;
        size++;
    }
    for (int i = size - 1; i >= 0; i--) {
        Render::putChar(buffer[i]);
    }
}

void Render::print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char* p = (char*) format;

    while (*p != '\0') {
        if (*p == '%') {
            switch (*(++p)) {
                case '\0':
                    return;
                case 'l':
                    switch (*(++p)) {
                        case '\0':
                            return;
                        case 'l':
                            switch (*(++p)) {
                                case 'u': // llu
                                    printUint64(va_arg(args, uint64_t));
                            }
                            break;
                    }
                    break;
                case 's':
                    printString(va_arg(args, const char*));
                    break;
            }
        } else {
            Render::putChar(*p);
        }
        p++;
    }
}


