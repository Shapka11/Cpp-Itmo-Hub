#include <iostream>
#include <cstring>
#include <string>

#include "Processor.h"


int main(int argc, const char** argv) {
    std::string input_file;
    std::string out_file;
    size_t out_address_begin;
    size_t out_size;
    bool is_out_data = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            ++i;
            input_file = argv[i];
        } else if (strcmp(argv[i], "-o") == 0) {
            try {
                out_file = argv[i + 1];
                out_address_begin = std::stoll(argv[i + 2], 0, 16);
                out_size = std::stoll(argv[i + 3]);
                i += 3;
                is_out_data = true;
            } catch (...) {
                std::cerr << "Error, incorrect argv" << std::endl;
                return 1;
            }
        }
    }

    CPU processor;
    processor.Emulation(input_file, CacheType::LRU);
    processor.ResetState();

    processor.Emulation(input_file, CacheType::BpLRU);
    processor.PrintStatistic();

    if (is_out_data) {
        processor.WriteInBinFile(out_file, out_address_begin, out_size);
    }

    return 0;
}
