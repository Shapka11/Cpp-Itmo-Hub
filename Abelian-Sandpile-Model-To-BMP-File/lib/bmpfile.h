#pragma once

#include <cinttypes>

#include "matrix.h"

#pragma pack(1)

struct BMPHeader {
    uint16_t bmp_file_type = 0x4D42;
    uint16_t reserved1 = 0;
    uint32_t bmp_file_size;
    uint32_t bfOffBits = 54 + 5 * 4;
    uint16_t reserved2 = 0;
};

struct BMPInfoHeader {
    uint32_t biSize = sizeof(BMPInfoHeader);
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 4;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = 0;
    int32_t biXPelsPerMeter = 0;
    int32_t biYPelsPerMeter = 0;
    uint32_t biClrUsed = 5;
    uint32_t biClrImportant = 0;
};

int UintLength(uint64_t value);

void CreateFileName(char *filename, const char *folder, uint64_t version);

void CreateBMP(Matrix& matrix, const char *folder);
