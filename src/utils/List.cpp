//
// Created by 屠昊天 on 2022/5/25.
//

#include <cassert>
#include "List.h"

template<typename T>
void List<T>::pushBack(T value) {
    auto element = new ListElement<T>{};
    element.value = value;

    if (this->start == nullptr) {
        this->start = element;
        this->end = element;
        return;
    }

    this->end->next = element;
    this->end = element;
}

template<typename T>
void List<T>::push(T value) {
    auto element = new ListElement<T>{};
    element.value = value;

    if (this->start == nullptr) {
        this->start = element;
        this->end = element;
        return;
    }

    element->next = this->start;
    this->start = element;
}

template<typename T>
T List<T>::pop() {
    assert(this->start != nullptr);
    assert(this->end != nullptr);

    auto value = start->value;
    auto next = start->next;
    delete this->start;
    this->start = next;
    return value;
}


template<typename T>
void List<T>::remove(size_t index) {
    size_t counter = index;

    if (index == 0) {
        assert(this->start != nullptr);
        auto next = start->next;
        delete this->start;
        this->start = next;
    }

    auto element = this->start;
    while (counter > 0) {
        assert(element != nullptr);
        if (counter == 1) {
            assert(element->next != nullptr);
            auto elementToDelete = element->next;
            element->next = elementToDelete->next;
            delete elementToDelete;
        }
        element = element->next;
        counter--;
    }
};







