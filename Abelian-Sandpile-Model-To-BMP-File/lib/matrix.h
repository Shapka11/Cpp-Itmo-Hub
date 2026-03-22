#pragma once

#include <cinttypes>

#include "stack.h"

struct Point {
    int16_t x = 0;
    int16_t y = 0;
    uint64_t count = 0;
};

struct Borders {
    int16_t x_min = INT16_MAX;
    int16_t x_max = 0;
    int16_t y_min = INT16_MAX;
    int16_t y_max = 0;
};

struct Matrix {
    uint64_t **value;
    uint32_t length;
    uint32_t width;
};

Point ParseLine(char *buffer);

Borders FirstParseFile(char *filename, Matrix& matrix);

void SecondParseFile(char *filename, Matrix& matrix, int16_t shift_x, int16_t shift_y, List& list);

void CreatePicture(Matrix& matrix, const char *folder);

void PushInStack(const uint8_t shift_y, const uint8_t shift_x, List& list, Matrix& matrix);

void CreateNewPicture(Matrix& matrix, List& list, uint64_t max_iter, uint64_t freq, const char *folder);
