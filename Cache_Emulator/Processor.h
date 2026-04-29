#pragma once

#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <stdio.h>

#include "CacheInterface.h"
#include "Cache.h"
#include "Consts.h"


enum class CacheType {
    LRU,
    BpLRU
};


class CPU {
public:
    void ResetState();
    void PrintStatistic();

    void WriteInBinFile(const std::string& filename, uint32_t address_start, uint32_t size);

    void Emulation(const std::string& filename, CacheType type);
    void ParsingInsreuctions(bool* data, CacheInterface& cache);

private:
    uint32_t pc = 0;
    uint32_t start_ra = 0;
    std::array<uint32_t, kRegistersCount> registers{};

    bool is_end_ = false;

    CacheLRU cache_lru;
    CacheBpLRU cache_bit_pLRU;

    uint8_t ReverseByte(uint8_t byte);
    uint32_t Slice(bool* data, uint8_t begin, uint8_t end);
    uint32_t Sext(uint32_t num, uint8_t size);
    void ParsingBinFile(const std::string& filename, CacheInterface& cache);
    void PrintInstruction(bool* data);

};
