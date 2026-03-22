#pragma once

#include <cinttypes>

struct Node {
    Node *next = NULL;
    int16_t x_value;
    int16_t y_value;
};

struct List {
    Node *head = NULL;
    Node *new_stack = NULL;
    void Push(int16_t x_value, int16_t y_value);
    void Pop();
    void DeleteList(Node *stack);
    bool IsEmpty();
};
