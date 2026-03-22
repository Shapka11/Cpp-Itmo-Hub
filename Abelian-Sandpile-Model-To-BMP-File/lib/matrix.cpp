#include <cinttypes>
#include <fstream>
#include <iostream>

#include "allocation.h"
#include "bmpfile.h"
#include "stack.h"
#include "matrix.h"

Point ParseLine(char *buffer) {
    uint8_t index_buffer = 0;
    uint8_t order_number = 1;
    bool is_positive = true;
    Point data_point;
    while (buffer[index_buffer] != '\0') {
        if (buffer[index_buffer] == '\t') {
            if (!is_positive) {
                if (order_number == 1) {
                    data_point.x *= -1;
                } else {
                    data_point.y *= -1;
                }
                is_positive = true;
            }
            ++order_number;
        } else if (buffer[index_buffer] == '-') {
            is_positive = false;
        } else if (order_number == 1) {
            data_point.x = data_point.x * 10 + (buffer[index_buffer] - '0');
        } else if (order_number == 2) {
            data_point.y = data_point.y * 10 + (buffer[index_buffer] - '0');
        } else {
            data_point.count = data_point.count * 10 + (buffer[index_buffer] - '0');
        }
        ++index_buffer;
    }
    return data_point;
}

Borders FirstParseFile(char *filename, Matrix& matrix) {
    std::ifstream file_input(filename);
    if (!file_input) {
        std::cerr << "Error!!! File does not exist!";
        std::exit(1);
    }
    const size_t buffer_size = 34;
    char buffer[buffer_size] {};
    Borders input_borders;
    while (file_input) {
        file_input.getline(buffer, buffer_size);
        if (!file_input) {
            break;
        }
        Point data = ParseLine(buffer);
        input_borders.x_max = std::max(input_borders.x_max, data.x);
        input_borders.x_min = std::min(input_borders.x_min, data.x);
        input_borders.y_max = std::max(input_borders.y_max, data.y);
        input_borders.y_min = std::min(input_borders.y_min, data.y);
    }
    file_input.close();
    matrix.length = input_borders.x_max - input_borders.x_min + 1;
    matrix.width = input_borders.y_max - input_borders.y_min + 1;
    return input_borders;
}

void SecondParseFile(char *filename, Matrix& matrix, int16_t shift_x, int16_t shift_y, List& list) {
    std::ifstream file_input(filename);
    if (!file_input) {
        std::cerr << "Error!!! File does not exist!";
        std::exit(1);
    }
    const size_t buffer_size = 34;
    char buffer[buffer_size] {};
    while (file_input) {
        file_input.getline(buffer, buffer_size);
        if (!file_input) {
            break;
        }
        Point data = ParseLine(buffer);
        matrix.value[data.y + shift_y][data.x + shift_x] = data.count;
            if (data.count > 3) {
            list.Push(data.x + shift_x, data.y + shift_y);
        }
    }
    list.head = list.new_stack;
    list.new_stack = NULL;
    file_input.close();
}

void CreatePicture(Matrix& matrix, const char *folder) {
    Matrix new_matrix;
    bool is_new_matrix = true;
    new_matrix.length = matrix.length;
    new_matrix.width = matrix.width;
    MatrixShift(new_matrix, matrix, is_new_matrix);
    CreateBMP(new_matrix, folder);
    if (is_new_matrix) {
        for (int i = 0; i < new_matrix.width; ++i) {
            delete []new_matrix.value[i];
        }
        delete []new_matrix.value;
    }
}

void PushInStack(const uint8_t shift_y, const uint8_t shift_x, List& list, Matrix& matrix) {
    const int16_t x = list.head->x_value;
    const int16_t y = list.head->y_value;
    matrix.value[y][x + 1] += 1;
    if (matrix.value[y][x + 1] == 4) {
        list.Push(x + 1 - shift_x, y - shift_y);
    }
    matrix.value[y][x - 1] += 1;
    if (matrix.value[y][x - 1] == 4) {
        list.Push(x - 1 - shift_x, y - shift_y);
    }
    matrix.value[y - 1][x] += 1;
    if (matrix.value[y - 1][x] == 4) {
        list.Push(x - shift_x, y - 1 - shift_y);
    }
    matrix.value[y + 1][x] += 1;
    if (matrix.value[y + 1][x] == 4) {
        list.Push(x - shift_x, y + 1 - shift_y);
    }
    matrix.value[y][x] -= 4;
    if (matrix.value[y][x] > 3) {
        list.Push(x - shift_x, y - shift_y);
    }
    list.Pop();
}

void CreateNewPicture(Matrix& matrix, List& list, uint64_t max_iter, uint64_t freq, const char *folder) {
    uint64_t index_iter = 0;
    uint8_t shift_y = 0;
    uint8_t shift_x = 0;

    while (index_iter < max_iter && list.IsEmpty() == false) {
        while (list.head != NULL) {
            list.head->x_value += shift_x;
            list.head->y_value += shift_y;
            if(matrix.value[list.head->y_value][list.head->x_value] < 4) {
                list.Pop();
                continue;
            }

            Allocation data_alloc;
            if (IsAllocate(matrix, data_alloc, list.head->y_value, list.head->x_value)) {
                uint8_t shift_y_after_alloc = 0;
                uint8_t shift_x_after_alloc = 0;
                MatrixAllocate(matrix, data_alloc, shift_y_after_alloc, shift_x_after_alloc);
                list.head->x_value += shift_x_after_alloc;
                list.head->y_value += shift_y_after_alloc;
                shift_x += shift_x_after_alloc;
                shift_y += shift_y_after_alloc;
            }

            PushInStack(shift_y, shift_x, list, matrix);
        }
        ++index_iter;
        list.head = list.new_stack;
        list.new_stack = NULL;
        if (freq != 0 && index_iter % freq == 0) {
            CreatePicture(matrix, folder);
        }
    }
    if (list.IsEmpty() == false) {
        list.DeleteList(list.new_stack);
    }
    if (freq == 0 || index_iter % freq != 0) {
        CreatePicture(matrix, folder);
    }
}
