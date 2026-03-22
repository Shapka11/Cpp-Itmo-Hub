#include <cinttypes>

#include "allocation.h"
#include "matrix.h"

bool IsAllocate(Matrix& matrix, Allocation& data_alloc, const uint32_t y, const uint32_t x) {
    bool is_alloc = false;
    if (y == 0) {
        data_alloc.is_up = true;
        is_alloc = true;
    }
    if (y == matrix.width - 1) {
        data_alloc.is_down = true;
        is_alloc = true;
    }
    if (x == 0) {
        data_alloc.is_left = true;
        is_alloc = true;
    }
    if (x == matrix.length - 1) {
        data_alloc.is_right = true;
        is_alloc = true;
    }
    if (is_alloc) {
        return true;
    } else {
        return false;
    }
}

void MatrixAllocLength(Matrix& matrix, const Allocation& data_sides) {
    uint64_t **new_matrix = new uint64_t*[matrix.width] {};
    uint32_t end = 0;
    uint8_t start = 0;
    if (data_sides.is_left && data_sides.is_right) {
        start = 1;
        end = matrix.length - 2;
    } else if (data_sides.is_right) {
        end = matrix.length - 2;
    } else {
        start = 1;
        end = matrix.length - 1;
    }
    for (int i = 0; i < matrix.width; ++i) {
        new_matrix[i] = new uint64_t[matrix.length] {};
        for (int j = (int)start; j <= (int)end; ++j) {
            new_matrix[i][j] = matrix.value[i][j - start];
        }
        delete []matrix.value[i];
    }
    matrix.value = new_matrix;
}

void MatrixAllocWidth(Matrix& matrix, const Allocation& data_sides) {
    uint64_t **new_matrix = new uint64_t*[matrix.width] {};
    uint32_t end = 0;
    uint8_t start = 0;
    if (data_sides.is_up && data_sides.is_down) {
        new_matrix[0] = new uint64_t[matrix.length] {};
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        end = matrix.width - 2;
        start = 1;
    } else if (data_sides.is_down) {
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        end = matrix.width - 2;
    } else {
        new_matrix[0] = new uint64_t[matrix.length] {};
        start = 1;
        end = matrix.width - 1;
    }
    for (int i = (int)start; i <= (int)end; ++i) {
        new_matrix[i] = matrix.value[i - start];
    }
    matrix.value = new_matrix;
}

void MatrixAllocAll(Matrix& matrix, const Allocation& data_sides) {
    uint64_t **new_matrix = new uint64_t*[matrix.width];
    uint8_t start_width = 0;
    uint8_t start_length = 0;
    uint32_t end_width = 0;
    uint32_t end_length = 0;
    if (data_sides.is_down && data_sides.is_left && data_sides.is_right && data_sides.is_up) {
        new_matrix[0] = new uint64_t[matrix.length] {};
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        start_length = 1;
        start_width = 1;
        end_length = matrix.length - 2;
        end_width = matrix.width - 2;
    } else if (data_sides.is_down && data_sides.is_up && data_sides.is_right) {
        new_matrix[0] = new uint64_t[matrix.length] {};
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        start_width = 1;
        end_length = matrix.length - 2;
        end_width = matrix.width - 2;
    } else if (data_sides.is_down && data_sides.is_up && data_sides.is_left) {
        new_matrix[0] = new uint64_t[matrix.length] {};
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        start_width = 1;
        start_length = 1;
        end_length = matrix.length - 1;
        end_width = matrix.width - 2;
    } else if (data_sides.is_left && data_sides.is_right && data_sides.is_up) {
        new_matrix[0] = new uint64_t[matrix.length] {};
        start_width = 1;
        start_length = 1;
        end_length = matrix.length - 2;
        end_width = matrix.width - 1;
    } else if (data_sides.is_left && data_sides.is_right && data_sides.is_down) {
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        start_length = 1;
        end_length = matrix.length - 2;
        end_width = matrix.width - 2;
    } else if (data_sides.is_left && data_sides.is_down) {
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        start_length = 1;
        end_length = matrix.length - 1;
        end_width = matrix.width - 2;
    } else if (data_sides.is_left && data_sides.is_up) {
        new_matrix[0] = new uint64_t[matrix.length] {};
        start_length = 1;
        end_length = matrix.length - 1;
        start_width = 1;
        end_width = matrix.width - 1;
    } else if (data_sides.is_right && data_sides.is_down) {
        new_matrix[matrix.width - 1] = new uint64_t[matrix.length] {};
        end_length = matrix.length - 2;
        end_width = matrix.width - 2;
    } else if (data_sides.is_right && data_sides.is_up) {
        new_matrix[0] = new uint64_t[matrix.length] {};
        end_length = matrix.length - 2;
        start_width = 1;
        end_width = matrix.width - 1;
    }
    for (int i = (int)start_width; i <= (int)end_width; ++i) {
        new_matrix[i] = new uint64_t[matrix.length] {};
        for (int j = (int)start_length; j <= (int)end_length; ++j) {
            new_matrix[i][j] = matrix.value[i - start_width][j - start_length];
        }
        delete []matrix.value[i - start_width];
    }
    matrix.value = new_matrix;
}

void MatrixAllocate(Matrix& matrix, Allocation& data_sides, uint8_t& shift_y, uint8_t& shift_x) {  
    if (data_sides.is_up) {
        ++shift_y;
        ++matrix.width;
    }
    if (data_sides.is_down) {
        ++matrix.width;
    }
    if (data_sides.is_left) {
        ++shift_x;
        ++matrix.length;
    }
    if (data_sides.is_right) {
        ++matrix.length;
    }
    if ((data_sides.is_down || data_sides.is_up) && (data_sides.is_left || data_sides.is_right)) {
        MatrixAllocAll(matrix, data_sides);
    } else if (data_sides.is_down || data_sides.is_up) {
        MatrixAllocWidth(matrix, data_sides);
    } else {
        MatrixAllocLength(matrix, data_sides);
    }

}

void MatrixShift(Matrix& new_matrix, Matrix& matrix, bool& is_new_matrix) {
    if (matrix.length % 8 != 0) {
        const uint8_t delta = 8 - (matrix.length % 8);
        new_matrix.value = new uint64_t*[matrix.width] {};
        for (int i = 0; i < matrix.width; ++i) {
            new_matrix.value[i] = new uint64_t[matrix.length + delta] {};
            for (int j = 0; j < matrix.length; ++j) {
                new_matrix.value[i][j] = matrix.value[i][j];
            }
        }
        new_matrix.length += delta;
    } else {
        new_matrix = matrix;
        is_new_matrix = false;
    }
}
