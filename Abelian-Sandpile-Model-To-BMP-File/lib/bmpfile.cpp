#include <cinttypes>
#include <iostream>

#include "bmpfile.h"
#include "matrix.h"

int UintLength(uint64_t value) {
    int result = 0;
    while (value > 0) {
        value /= 10;
        ++result;
    }
    return result;
}

void CreateFileName(char *filename, const char *folder, uint64_t version) {
    for (int i = 0; i < std::strlen(folder); ++i) {
        filename[i] = folder[i];
    }
    int index_filename = std::strlen(folder);
    const char *name = "\\picture_version_";
    for (int i = index_filename; i < index_filename + 17; ++i) {
        filename[i] = name[i % index_filename];
    }
    index_filename += 17;
    const int length_number = UintLength(version);
    for (int i = index_filename + length_number - 1; i >= index_filename; --i) {
        filename[i] = version % 10 + '0';
        version /= 10;
    }
    index_filename += length_number;
    const char *type = ".bmp";
    for (int i = index_filename; i < index_filename + 4; ++i) {
        filename[i] = type[i % index_filename];
    }
    index_filename += 4;
    filename[index_filename] = '\0';
}

void CreateBMP(Matrix& matrix, const char *folder) {
    const int width = matrix.length;  
    const int height = matrix.width;
    const size_t row_size = width / 2; 

    BMPHeader bmpHeader;
    BMPInfoHeader infoHeader;
    
    bmpHeader.bmp_file_size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + row_size * height;
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;

    uint8_t palette[5][4] = {
        {255, 255, 255, 0}, 
        {0, 128, 0, 0},     
        {128, 0, 128, 0},   
        {0, 255, 255, 0},   
        {0, 0, 0, 0}
    };
    
    static uint64_t version = 1;
    const uint8_t length_file_name = 21;
    char *filename = new char[std::strlen(folder) + UintLength(version) + length_file_name + 1];
    CreateFileName(filename, folder, version);
    ++version;
    FILE *file = fopen(filename, "wb");
    fwrite(&bmpHeader,  sizeof(bmpHeader), 1, file); 
    fwrite(&infoHeader,  sizeof(infoHeader), 1, file); 

    for (int i = 0; i < 5; i++) {
        fwrite(&palette[i], sizeof(palette[i]), 1, file);
    }

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; x += 2) {
            if (matrix.value[y][x] > 3) {
                matrix.value[y][x] = 4;
            }
            if (matrix.value[y][x + 1] > 3) {
                matrix.value[y][x + 1] = 4;
            }
            uint8_t pixel_value = (matrix.value[y][x] * 16) + matrix.value[y][x + 1];
            fwrite(&pixel_value, sizeof(pixel_value), 1, file);
        }
    }
    fclose(file);
    delete []filename;
}
