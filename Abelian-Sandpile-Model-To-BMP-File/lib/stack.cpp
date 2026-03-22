#include <cinttypes>
#include <iostream>

#include "stack.h"

void List::Push(int16_t x_value, int16_t y_value) {
    Node *new_node = new Node();
    if (new_stack == NULL) {
        new_stack = new_node;
        new_node->x_value = x_value;
        new_node->y_value = y_value;
    } else {
        new_node->next = new_stack;
        new_stack = new_node;
        new_stack->x_value = x_value;
        new_stack->y_value = y_value;
    }
}

void List::Pop() {
    Node *temp = head;
    head = head->next;
    delete temp;
}

void List::DeleteList(Node *stack) {
    while (stack != NULL) {
        Node *temp = stack;
        stack = stack->next;
        delete temp;
    }
}

bool List::IsEmpty() {
    if (head == NULL) {
        return true;
    } else {
        return false;
    }
}
