#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>

#include "CacheInterface.h"
#include "Consts.h"
#include "RAM.h"


class Cache : public CacheInterface {
public:
    Cache();
    ~Cache();

    double GetDataStatistic();
    double GetInstrStatistic();
    double GetAllStatistic();

    void LoadByteToRAM(const uint32_t index, const uint8_t value);
    uint8_t UnloadByteRAM(const uint32_t index);

    uint32_t GetBytes(const uint32_t address, const uint8_t amount_bytes, const bool is_instr);
    void SetBytes(const uint32_t address, const uint32_t value, const uint8_t amount_bytes);

    void ClearCache();

protected:
    struct Block {
        uint8_t tag = 0;
        uint64_t time = 0;
        bool valid = false;
        bool dirty = false; // true -> данные нужно записать в ram

        uint8_t line[CACHE_LINE_SIZE];
    };

    struct DataAddress {
        uint32_t tag = 0;
        uint32_t index = 0;
        uint32_t offset = 0;
    };

    DataAddress data_address_;
    Block **data_ = nullptr;
    RAM ram_;

    size_t data_hit_ = 0;
    size_t data_miss_ = 0;
    size_t instr_hit_ = 0;
    size_t instr_miss_ = 0;

    virtual void UploadCacheLine(uint32_t address, Block& cache_line);
    void TimeUpdate(uint8_t index_basket, uint8_t index_new_block);
    bool IsBasketFull();
    int FindLine(); 
    int FindEmptyLine();
    void InstallAddressData(uint32_t address);

};


class CacheLRU : public Cache {
public:
    CacheLRU() = default;
    ~CacheLRU() = default;

private:
    void TimeUpdate(uint8_t index_basket, uint8_t index_new_block);
    void PolicyCache(uint8_t index_basket);
    std::string GetType();

};


class CacheBpLRU : public Cache {
public:
    CacheBpLRU() = default;
    ~CacheBpLRU() = default;

private:
    void TimeUpdate(uint8_t index_basket, uint8_t index_new_block);
    void PolicyCache(uint8_t index_basket);
    std::string GetType();

};
