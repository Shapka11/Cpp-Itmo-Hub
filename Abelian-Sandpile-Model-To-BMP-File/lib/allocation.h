#pragma once

#include <cinttypes>

#include "matrix.h"

struct Allocation {
    bool is_left = false;
    bool is_up = false;
    bool is_right = false;
    bool is_down = false;
};

bool IsAllocate(Matrix& matrix, Allocation& data_alloc, const uint32_t y, const uint32_t x);

void MatrixAllocLength(Matrix& matrix, const Allocation& data_sides);

void MatrixAllocWidth(Matrix& matrix, const Allocation& data_sides);

void MatrixAllocAll(Matrix& matrix, const Allocation& data_sides);

void MatrixAllocate(Matrix& matrix, Allocation& data_sides, uint8_t& shift_y, uint8_t& shift_x);

void MatrixShift(Matrix& new_matrix, Matrix& matrix, bool& is_new_matrix);
