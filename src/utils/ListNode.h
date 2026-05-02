//
// Created by 屠昊天 on 2026/5/2.
//


#pragma once

#include <stddef.h>

struct ListNode {
    ListNode* prev;
    ListNode* next;
};

#define container_of(ptr, type, member) \
    ((type*)((char*)(ptr) - offsetof(type, member)))

// 初始化哨兵节点：prev 和 next 都指向自己，表示空队列
inline void list_init(ListNode* node) {
    node->prev = node;
    node->next = node;
}

// 插入到 head 之后（头插）
inline void list_push(ListNode* head, ListNode* node) {
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

// 插入到 head 之前（尾插）
inline void list_pushBack(ListNode* head, ListNode* node) {
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}

// 将节点从队列中移除（O(1)，不需要知道在哪个队列里）
inline void list_remove(ListNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// 队列是否为空
inline bool list_empty(ListNode* head) {
    return head->next == head;
}

// 遍历队列（不能在循环内对当前节点调用 list_remove）
#define list_foreach(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
