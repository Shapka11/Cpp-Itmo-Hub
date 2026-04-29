#pragma once

#include <cstdint>
#include <string>


class CacheInterface {
public:
    virtual ~CacheInterface() = default;
    virtual uint32_t GetBytes(uint32_t address, uint8_t amount_bytes, bool is_instr) = 0;
    virtual void SetBytes(uint32_t address, uint32_t value, uint8_t amount_bytes) = 0;
    virtual void LoadByteToRAM(uint32_t index, uint8_t value) = 0;
    virtual uint8_t UnloadByteRAM(uint32_t index) = 0;
    virtual double GetDataStatistic() = 0;
    virtual double GetInstrStatistic() = 0;
    virtual double GetAllStatistic() = 0;
    virtual void ClearCache() = 0;
protected:
    virtual void TimeUpdate(uint8_t index_basket, uint8_t index_new_block) = 0;
    virtual bool IsBasketFull() = 0;
    virtual int FindLine() = 0;
    virtual int FindEmptyLine() = 0;
    virtual void PolicyCache(uint8_t index_basket) = 0;
    virtual void InstallAddressData(uint32_t address) = 0;
    virtual std::string GetType() = 0;
};
