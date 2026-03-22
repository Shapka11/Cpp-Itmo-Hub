#include <iostream>
#include <cinttypes>

#include "parsing.h"

bool StringCompare(const char *value1, const char *value2) {
    uint8_t index = 0;
    while (value1[index] != '\0' && value2[index] != '\0') {
        if (value1[index] != value2[index]) {
            return false;
        }
        ++index;
    }
    if (value1[index] == '\0' && value2[index] == '\0') {
        return true;
    } else {
        return false;
    }
}

Arguments ParseArguments(int argc, char **argv) {
    Arguments parameters;
    for (int i = 1; i < argc; i++) {
        if (StringCompare(argv[i], "-o") || StringCompare(argv[i], "--output")) {
            parameters.filename_output = argv[i + 1];
            ++i;
        } else if (StringCompare(argv[i], "-i") || StringCompare(argv[i], "--input")) {
            parameters.filename_input = argv[i + 1];
            ++i;
        } else if (StringCompare(argv[i], "-m") || StringCompare(argv[i], "--max-iter")) {
            parameters.iterations_count = atoi(argv[i + 1]);
            ++i;
        } else if (StringCompare(argv[i], "-f") || StringCompare(argv[i], "--freq")) {
            parameters.frequency_save = atoi(argv[i + 1]);
            ++i;
        } else {
            std::cerr << "Unknow parameter!";
            exit(EXIT_FAILURE);
        }
    }
    return parameters;
}
