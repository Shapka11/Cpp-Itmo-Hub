#pragma once

#include <cstdint>
#include <vector>

#include "Consts.h"


class RAM {
public:
    RAM();

    void SetByte(const uint32_t index, const uint8_t value);
    uint8_t GetByte(const uint32_t index);

private:
    std::vector<uint8_t> data_;

};
