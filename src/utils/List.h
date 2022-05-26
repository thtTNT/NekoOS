//
// Created by 屠昊天 on 2022/5/25.
//


#pragma once

#include <cstddef>

template<typename T>
struct ListElement {
    T value;
    ListElement* next;
};

template<typename T>
class List {
    ListElement<T>* start;
    ListElement<T>* end;

public:
    void pushBack(T value);

    void push(T value);

    T pop();

    void remove(size_t index);
};
