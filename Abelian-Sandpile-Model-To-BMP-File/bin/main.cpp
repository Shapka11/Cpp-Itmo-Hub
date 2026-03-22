#include <cinttypes>
#include <iostream>

#include "lib/matrix.h"
#include "lib/parsing.h"
#include "lib/stack.h"

int main(int argc, char **argv) {
    Arguments input_data = ParseArguments(argc, argv);
    Matrix matrix;
    Borders matrix_data = FirstParseFile(input_data.filename_input, matrix);
    List list;

    matrix.value = new uint64_t*[matrix.width];
    for (int i = 0; i < matrix.width; ++i) {
        matrix.value[i] = new uint64_t[matrix.length] {};
    }
    SecondParseFile(input_data.filename_input, matrix, 0 - matrix_data.x_min, 0 - matrix_data.y_min, list);
    CreateNewPicture(matrix, list, input_data.iterations_count, input_data.frequency_save, input_data.filename_output);
    
    for (int i = 0; i < (int)matrix.width; ++i) {
        delete []matrix.value[i];
    }
    delete []matrix.value;

    std::cout<<"All pictures created :)";
    return 0;
}
