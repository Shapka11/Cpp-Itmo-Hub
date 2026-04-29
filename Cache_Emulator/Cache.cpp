#include "Cache.h"


Cache::Cache() {
    data_ = new Block *[CACHE_SET_COUNT] {};

    for (uint32_t i = 0; i < CACHE_SET_COUNT; ++i) {
        data_[i] = new Block[CACHE_WAY] {};
    }
}

Cache::~Cache() {
    for (uint32_t i = 0; i < CACHE_SET_COUNT; ++i) {
        delete[] data_[i];
    }
    delete[] data_;
}


double Cache::GetDataStatistic() {
    return std::abs(((double)data_hit_ / (double)(data_hit_ + data_miss_)) * 100);
}

double Cache::GetInstrStatistic() {
    return std::abs(((double)instr_hit_ / (double)(instr_hit_ + instr_miss_)) * 100);
}

double Cache::GetAllStatistic() {
    return std::abs(((double)(instr_hit_ + data_hit_) / (double)(instr_hit_ + instr_miss_ + data_hit_ + data_miss_)) * 100);
}


void Cache::LoadByteToRAM(const uint32_t index, const uint8_t value) {
    ram_.SetByte(index, value);
}

uint8_t Cache::UnloadByteRAM(const uint32_t index) {
    return ram_.GetByte(index);
}


uint32_t Cache::GetBytes(const uint32_t address, const uint8_t amount_bytes, const bool is_instr) {
    InstallAddressData(address);
    uint32_t result = 0;

    // поиск данных в кэшэ
    int8_t num_line = FindLine();
    if (num_line != -1) {
        TimeUpdate(data_address_.index, num_line);
        is_instr ? ++instr_hit_ : ++data_hit_;
        for (uint8_t i = 0; i < amount_bytes; ++i) {
            result = (result << 8) + data_[data_address_.index][num_line].line[data_address_.offset + i];
        }
        if (IsBasketFull() && GetType() == "BpLRU") {
            for (uint32_t i = 0; i < CACHE_WAY; ++i) { data_[data_address_.index][i].time = 0; }
            TimeUpdate(data_address_.index, num_line);
        }
        return result;
    }

    is_instr ? ++instr_miss_ : ++data_miss_;
    if (IsBasketFull()) { // basket полный
        PolicyCache(data_address_.index); // политика вытеснения
    }

    // заполнение освободившейся line
    num_line = FindEmptyLine();
    Block &cache_line = data_[data_address_.index][num_line];
    cache_line.dirty = false;
    cache_line.valid = true;
    cache_line.tag = data_address_.tag;
    UploadCacheLine(address, cache_line);
    TimeUpdate(data_address_.index, num_line);
    for (uint8_t i = 0; i < amount_bytes; ++i) {
        result = (result << 8) + data_[data_address_.index][num_line].line[data_address_.offset + i];
    }
    if (IsBasketFull() && GetType() == "BpLRU") {
        for (uint32_t i = 0; i < CACHE_WAY; ++i) { data_[data_address_.index][i].time = 0; }
        TimeUpdate(data_address_.index, num_line);
    }
    return result;
}

void Cache::SetBytes(const uint32_t address, const uint32_t value, const uint8_t amount_bytes) {
    InstallAddressData(address);
    const uint8_t value_size = amount_bytes * 8;

    // проверка, что уже address есть в cache
    int8_t num_line = FindLine();
    if (num_line != -1) {
        Block &cache_line = data_[data_address_.index][num_line];
        for (uint8_t i = 1; i <= amount_bytes; ++i) {
            cache_line.line[data_address_.offset + i - 1] = (value >> (value_size - 8 * i)) & 0xFF;
        }
        TimeUpdate(data_address_.index, num_line);
        cache_line.dirty = true;
        cache_line.valid = true;
        ++data_hit_;
        if (IsBasketFull() && GetType() == "BpLRU") {
            for (uint32_t i = 0; i < CACHE_WAY; ++i) { data_[data_address_.index][i].time = 0; }
            TimeUpdate(data_address_.index, num_line);
        }
        return;
    }

    ++data_miss_;
    if (IsBasketFull()) { // basket полный
        PolicyCache(data_address_.index); // политика вытеснения
    }

    // заполнение освободившейся line
    num_line = FindEmptyLine();
    Block &cache_line = data_[data_address_.index][num_line];
    cache_line.dirty = true;
    cache_line.valid = true;
    cache_line.tag = data_address_.tag;
    UploadCacheLine(address, cache_line);
    TimeUpdate(data_address_.index, num_line);
    for (uint8_t i = 1; i <= amount_bytes; ++i) {
        cache_line.line[data_address_.offset + i - 1] = (value >> (value_size - 8 * i)) & 0xFF;
    }
    if (IsBasketFull() && GetType() == "BpLRU") {
        for (uint32_t i = 0; i < CACHE_WAY; ++i) { data_[data_address_.index][i].time = 0; }
        TimeUpdate(data_address_.index, num_line);
    }
}


void Cache::ClearCache() {
    for (size_t i = 0; i < CACHE_SET_COUNT; ++i) {
        for (size_t j = 0; j < CACHE_WAY; ++j) {
            if (data_[i][j].valid) { // запись в ram
                uint32_t address = (data_[i][j].tag << (CACHE_OFFSET_LEN + CACHE_INDEX_LEN)) + (i << CACHE_OFFSET_LEN);
                for (size_t k = 0; k < CACHE_LINE_SIZE; ++k) {
                    ram_.SetByte(address + k, data_[i][j].line[k]);
                }
            }
        }
    }
}


void Cache::UploadCacheLine(uint32_t address, Block &cache_line) {
    const uint32_t address_start_line = (address >> CACHE_OFFSET_LEN) << CACHE_OFFSET_LEN; // адрес начала cache line в RAM
    for (size_t i = 0; i < CACHE_LINE_SIZE; ++i) {
        cache_line.line[i] = ram_.GetByte(address_start_line + i);
    }
}

void Cache::TimeUpdate(uint8_t index_basket, uint8_t index_new_block) {
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        ++data_[index_basket][i].time;
    }
    data_[index_basket][index_new_block].time = 0;
}

bool Cache::IsBasketFull() {
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        if (data_[data_address_.index][i].valid == false) {
            return false;
        }
    }
    return true;
}

int Cache::FindLine() {
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        if (data_[data_address_.index][i].tag == data_address_.tag && data_[data_address_.index][i].valid) {
            return i;
        }
    }
    return -1;
}

int Cache::FindEmptyLine() {
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        if (!data_[data_address_.index][i].valid) {
            return i;
        }
    }
    return -1;
}

void Cache::InstallAddressData(uint32_t address) {
    const uint32_t maska_index = (1 << CACHE_INDEX_LEN) - 1;
    const uint32_t maska_offset = (1 << CACHE_OFFSET_LEN) - 1;
    const uint32_t maska_tag = (1 << CACHE_TAG_LEN) - 1;

    data_address_.tag = (address >> (CACHE_INDEX_LEN + CACHE_OFFSET_LEN)) & maska_tag;
    data_address_.index = (address >> CACHE_OFFSET_LEN) & maska_index;
    data_address_.offset = address & maska_offset;
}





void CacheLRU::TimeUpdate(uint8_t index_basket, uint8_t index_new_block) {
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        ++data_[index_basket][i].time;
    }
    data_[index_basket][index_new_block].time = 0;
}

void CacheLRU::PolicyCache(uint8_t index_basket) {
    uint8_t index_old_block = 0;
    uint64_t max_time = 0;
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        if (data_[index_basket][i].time > max_time) {
            max_time = data_[index_basket][i].time;
            index_old_block = i;
        }
    }

    if (data_[index_basket][index_old_block].dirty) { // обновить данные в RAM
        uint32_t address = (data_[index_basket][index_old_block].tag << (CACHE_OFFSET_LEN + CACHE_INDEX_LEN))
            + (index_basket << CACHE_OFFSET_LEN);
        for (uint8_t i = 0; i < CACHE_LINE_SIZE; ++i) {
            ram_.SetByte(address + i, data_[index_basket][index_old_block].line[i]);
        }
    }

    data_[index_basket][index_old_block].dirty = false;
    data_[index_basket][index_old_block].valid = false;
}

std::string CacheLRU::GetType() {
    return "LRU";
}



void CacheBpLRU::TimeUpdate(uint8_t index_basket, uint8_t index_new_block) {
    data_[index_basket][index_new_block].time = 1;
}

void CacheBpLRU::PolicyCache(uint8_t index_basket) {
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        if (data_[index_basket][i].time == 0) { // этот cache_line вытесняется
            data_[index_basket][i].dirty = false;
            data_[index_basket][i].valid = false;
            return;
        }
    }

    // все time == 1 -> все time = 0
    for (uint8_t i = 0; i < CACHE_WAY; ++i) {
        data_[index_basket][i].time = 0;
        if (i == 0) { // этот cache_line вытесняется
            data_[index_basket][i].dirty = false;
            data_[index_basket][i].valid = false;
        }
    }
}

std::string CacheBpLRU::GetType() {
    return "BpLRU";
}
