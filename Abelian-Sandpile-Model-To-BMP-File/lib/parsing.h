#pragma once

#include <cinttypes>

struct Arguments { 
    char *filename_input;
    char *filename_output;
    uint64_t iterations_count = 0;
    uint64_t frequency_save = 0;
};

bool StringCompare(const char *value1, const char *value2);

Arguments ParseArguments(int argc, char **argv);
