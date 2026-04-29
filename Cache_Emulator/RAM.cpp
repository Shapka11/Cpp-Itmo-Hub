#include "RAM.h"


RAM::RAM() {
    data_.resize(MEMORY_SIZE);
}

void RAM::SetByte(const uint32_t index, const uint8_t value) {
    data_[index] = value;
}

uint8_t RAM::GetByte(const uint32_t index) {
    return data_[index];
}
