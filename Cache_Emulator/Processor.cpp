#include "Processor.h"

#include <cstddef>
#include <cstring>


void CPU::ResetState() {
    pc = 0;
    start_ra = 0;
    is_end_ = false;
    registers.fill(0);
}

void CPU::PrintStatistic() {
    printf("replacement\thit rate\thit rate (inst)\thit rate (data)\n"
        "        LRU\t%3.5f%%\t%3.5f%%\t%3.5f%%\n"
        "      bpLRU\t%3.5f%%\t%3.5f%%\t%3.5f%%\n", 
        cache_lru.GetAllStatistic(), cache_lru.GetInstrStatistic(), cache_lru.GetDataStatistic(),
        cache_bit_pLRU.GetAllStatistic(), cache_bit_pLRU.GetInstrStatistic(), cache_bit_pLRU.GetDataStatistic()
    );
}

void CPU::WriteInBinFile(const std::string &filename, uint32_t address_start, uint32_t size) {
    std::ofstream file(filename, std::ios::binary);

    file.write(reinterpret_cast<char*>(&pc), sizeof(pc));
    for (size_t i = 1; i < kRegistersCount; ++i) {
        file.write(reinterpret_cast<char*>(&registers[i]), sizeof(registers[i]));
    }

    file.write(reinterpret_cast<char*>(&address_start), sizeof(address_start));
    file.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (size_t i = 0; i < size; ++i) {
        uint8_t ram_data = cache_lru.UnloadByteRAM(address_start + i);
        file.write(reinterpret_cast<char*>(&ram_data), sizeof(ram_data));
    }

    file.close();
}

void CPU::Emulation(const std::string &filename, CacheType type) {
    CacheInterface& cache = (type == CacheType::LRU) ? 
            static_cast<CacheInterface&>(cache_lru) : 
            static_cast<CacheInterface&>(cache_bit_pLRU);
    ParsingBinFile(filename, cache);
    start_ra = registers[1];

    while (!is_end_ && pc != start_ra) {
        uint32_t old_pc = pc;
        const bool is_instr = true;
        uint32_t instruction = cache.GetBytes(pc, kInstrSizeByte, is_instr);
        for (uint8_t i = 0; i < kInstrSizeByte; ++i) {
            const uint8_t byte = (instruction >> 8 * i) & 0xFF;
            instruction -= byte << 8 * i;
            instruction += (ReverseByte(byte) << 8 * i);
        }
        bool instr[kInstrSizeBit];
        for (size_t i = 0; i < kInstrSizeBit; ++i) {
            instr[i] = (instruction >> (kInstrSizeBit - 1 - i)) & 1;
        }
        ParsingInsreuctions(instr, cache);
        if (old_pc == pc) { pc += 4; }
    }
    if (is_end_) { pc -=4; }
    cache.ClearCache();
}


void CPU::ParsingInsreuctions(bool *data, CacheInterface &cache) {
    uint8_t rd = Slice(data, 7, 11);
    uint32_t value_0_to_6 = Slice(data, 0, 6);
    
    if (value_0_to_6 == 0b0110111) { // lui
        uint32_t imm = Slice(data, 12, 31);
        registers[rd] = (imm << 12);
        return;
    } else if (value_0_to_6 == 0b0010111) { // auipc
        uint32_t imm = Slice(data, 12, 31);
        registers[rd] = pc + (imm << 12);
        return;
    }

    uint32_t value_12_to_14 = Slice(data, 12, 14);
    uint8_t rs1 = Slice(data, 15, 19);
    uint32_t imm = Slice(data, 20, 31);
    if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b000) { // addi
        registers[rd] = registers[rs1] + Sext(imm, 12);
        return;
    } else if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b010) { // slti
        registers[rd] = static_cast<int32_t>(registers[rs1]) < static_cast<int32_t>(imm);
        return;
    } else if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b011) { // sltiu
        registers[rd] = registers[rs1] < imm;
        return;
    } else if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b100) { // xori
        registers[rd] = registers[rs1] ^ Sext(imm, 12);
        return;
    } else if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b110) { // ori
        registers[rd] = registers[rs1] | Sext(imm, 12);
        return;
    } else if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b111) { // andi
        registers[rd] = registers[rs1] & Sext(imm, 12);
        return;
    } 
    
    uint32_t shamt = Slice(data, 20, 24) & 0b11111;
    if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b001 && Slice(data, 26, 31) == 0) { // slli
        registers[rd] = registers[rs1] << shamt;
        return;
    } else if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b101 && Slice(data, 26, 31) == 0) { // srli
        registers[rd] = registers[rs1] >> shamt;
        return;
    } else if (value_0_to_6 == 0b0010011 && value_12_to_14 == 0b101 && Slice(data, 26, 31) == 0b010000) { // srai
        registers[rd] = static_cast<int32_t>(registers[rs1]) >> shamt;
        return;
    }

    uint32_t value_25_to_31 = Slice(data, 25, 31);
    uint8_t rs2 = Slice(data, 20, 24);
    if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0 && value_25_to_31 == 0) { // add
        registers[rd] = registers[rs1] + registers[rs2];
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0 && value_25_to_31 == 0b0100000) { // sub
        registers[rd] = registers[rs1] - registers[rs2];
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b001 && value_25_to_31 == 0) { // sll
        registers[rd] = registers[rs1] << (registers[rs2] & 0b11111);
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b010 && value_25_to_31 == 0) { // stl
        registers[rd] = static_cast<int16_t>(registers[rs1]) < static_cast<int32_t>(registers[rs2]);
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b011 && value_25_to_31 == 0) { // stlu
        registers[rd] = registers[rs1] < registers[rs2];
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b100 && value_25_to_31 == 0) { // xor
        registers[rd] = registers[rs1] ^ registers[rs2];
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b101 && value_25_to_31 == 0) { // srl
        registers[rd] = registers[rs1] >> (registers[rs2] & 0b11111);
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b101 && value_25_to_31 == 0b0100000) { // sra
        registers[rd] = static_cast<int32_t>(registers[rs1]) >> (registers[rs2] & 0b11111);
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b110 && value_25_to_31 == 0) { // or
        registers[rd] = registers[rs1] | registers[rs2];
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b111 && value_25_to_31 == 0) { // and
        registers[rd] = registers[rs1] & registers[rs2];
        return;
    }
    
    if (Slice(data, 0, 19) == 0xF && Slice(data, 28, 31) == 0) { // fence
        pc += 4;
        return;
    }

    if (Slice(data, 0, 31) == 0b1110011) { // ecall
        is_end_ = true;
        return;
    } else if (Slice(data, 0, 31) == 0x100073) { // ebreak
        is_end_ = true;
        return;
    }

    uint32_t offset = Slice(data, 20, 31);
    uint32_t address = registers[rs1] + Sext(offset, 12);
    const bool is_get_instr = false;
    if (value_0_to_6 == 0b11 && value_12_to_14 == 0) { // lb
        registers[rd] = static_cast<int32_t>(static_cast<int8_t>(cache.GetBytes(address, 1, is_get_instr)));
        return;
    } else if (value_0_to_6 == 0b11 && value_12_to_14 == 0b001) { // lh
        registers[rd] = static_cast<int32_t>(static_cast<int16_t>(cache.GetBytes(address, 2, is_get_instr)));
        return;
    } else if (value_0_to_6 == 0b11 && value_12_to_14 == 0b010) { // lw
        registers[rd] = static_cast<int32_t>(cache.GetBytes(address, 4, is_get_instr));
        return;
    } else if (value_0_to_6 == 0b11 && value_12_to_14 == 0b100) { // lbu
        registers[rd] = cache.GetBytes(address, 1, is_get_instr);
        return;
    } else if (value_0_to_6 == 0b11 && value_12_to_14 == 0b101) { // lhu
        registers[rd] = cache.GetBytes(address, 2, is_get_instr);
        return;
    }
    
    offset = (Slice(data, 25, 31) << 5) + Slice(data, 7, 11);
    rs2 = Slice(data, 20, 24);
    address = registers[rs1] + Sext(offset, 12);
    if (value_0_to_6 == 0b0100011 && value_12_to_14 == 0) { // sb
        cache.SetBytes(address, registers[rs2], 1);
        return;
    } else if (value_0_to_6 == 0b0100011 && value_12_to_14 == 0b001) { // sh
        cache.SetBytes(address, registers[rs2], 2);
        return;
    } else if (value_0_to_6 == 0b0100011 && value_12_to_14 == 0b010) { // sw
        cache.SetBytes(address, registers[rs2], 4);
        return;
    }

    if (value_0_to_6 == 0b1101111) { // jal
        offset = (Slice(data, 21, 30) << 1) + (Slice(data, 20, 20) << 11) 
            + (Slice(data, 12, 19) << 12) + (Slice(data, 31, 31) << 20);
        registers[rd] = pc + 4;
        pc += Sext(offset, 21);
        return;
    } else if (value_0_to_6 == 0b1100111 && value_12_to_14 == 0) { // jalr
        offset = Slice(data, 20, 31);
        uint32_t temp = pc + 4;
        pc = (registers[rs1] + Sext(offset, 12)) & ~1;
        registers[rd] = temp;
        return;
    }

    offset = (Slice(data, 8, 11) << 1) + (Slice(data, 25, 30) << 5) + 
        (Slice(data, 7, 7) << 11) + (Slice(data, 31, 31) << 12);
    offset = Sext(offset, 13);

    if (value_0_to_6 == 0b1100011 && value_12_to_14 == 0) { // beq
        pc = (registers[rs1] == registers[rs2]) ? pc + offset : pc;
        return;
    } else if (value_0_to_6 == 0b1100011 && value_12_to_14 == 1) { // bne
        pc = !(registers[rs1] == registers[rs2]) ? pc + offset : pc;
        return;
    } else if (value_0_to_6 == 0b1100011 && value_12_to_14 == 0b100) { // blt
        pc = (static_cast<int32_t>(registers[rs1]) < static_cast<int32_t>(registers[rs2])) ? pc + offset : pc;
        return;
    } else if (value_0_to_6 == 0b1100011 && value_12_to_14 == 0b101) { // bge
        pc = (static_cast<int32_t>(registers[rs1]) >= static_cast<int32_t>(registers[rs2])) ? pc + offset : pc;
        return;
    } else if (value_0_to_6 == 0b1100011 && value_12_to_14 == 0b110) { // bltu
        pc = (registers[rs1] < registers[rs2]) ? pc + offset : pc;
        return;
    } else if (value_0_to_6 == 0b1100011 && value_12_to_14 == 0b111) { // bgeu
        pc = (registers[rs1] >= registers[rs2]) ? pc + offset : pc;
        return;
    }

    //  RV32M
    if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0 && value_25_to_31 == 1) { // mul
        registers[rd] = static_cast<int64_t>(registers[rs1]) * static_cast<int64_t>(registers[rs2]);
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b001 && value_25_to_31 == 1) { // mulh
        registers[rd] = (static_cast<int64_t>(registers[rs1]) * static_cast<int64_t>(registers[rs2])) >> 32;
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b010 && value_25_to_31 == 1) { // mulhsu
        registers[rd] = (static_cast<int64_t>(registers[rs1]) * static_cast<uint64_t>(registers[rs2])) >> 32;
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b011 && value_25_to_31 == 1) { // mulhu
        registers[rd] = (static_cast<uint64_t>(registers[rs1]) * static_cast<uint64_t>(registers[rs2])) >> 32;
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b100 && value_25_to_31 == 1) { // div
        registers[rd] = static_cast<int32_t>(registers[rs1]) / static_cast<int32_t>(registers[rs2]);
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b101 && value_25_to_31 == 1) { // divu
        registers[rd] = registers[rs1] / registers[rs2];
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b110 && value_25_to_31 == 1) { // rem
        registers[rd] = static_cast<int32_t>(registers[rs1]) % static_cast<int32_t>(registers[rs2]);
        return;
    } else if (value_0_to_6 == 0b0110011 && value_12_to_14 == 0b111 && value_25_to_31 == 1) { // remu
        registers[rd] = registers[rs1] % registers[rs2];
        return;
    }

    PrintInstruction(data);
    std::cerr << " Commant wasn't find in insrtuctions!" << std::endl;;
}

uint8_t CPU::ReverseByte(uint8_t byte) {
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

uint32_t CPU::Slice(bool *data, uint8_t begin, uint8_t end) {
    uint32_t res = 0;
    uint8_t index = 0;

    for (uint8_t i = 0; i < kInstrSizeBit; ++i) {
        if (i >= begin && i <= end) {
            res += (data[i] << index);
            ++index;
        }
    }
    return res;
}

uint32_t CPU::Sext(uint32_t num, uint8_t size) {
    uint8_t shift = kRegisterSize - size;
    return static_cast<int32_t>(num << shift) >> shift;
}

void CPU::ParsingBinFile(const std::string &filename, CacheInterface &cache) {
    std::ifstream file(filename, std::ios::binary);

    file.read(reinterpret_cast<char*>(&pc), sizeof(pc));
    for (size_t i = 1; i < kRegistersCount; ++i) {
        file.read(reinterpret_cast<char*>(&registers[i]), sizeof(registers[i]));
    }

    while (file.peek() != EOF) {
        uint32_t address_begin, size_fragment;
        file.read(reinterpret_cast<char*>(&address_begin), sizeof(address_begin));
        file.read(reinterpret_cast<char*>(&size_fragment), sizeof(size_fragment));   
    
        for (uint32_t i = 0; i < size_fragment; i += kInstrSizeByte) {
            uint8_t buffer[kInstrSizeByte];
            file.read(reinterpret_cast<char*>(buffer), kInstrSizeByte);

            for (size_t j = 0; j < kInstrSizeByte; ++j) {
                cache.LoadByteToRAM(address_begin + j + i, buffer[j]);
            }
        }
    }

    file.close();
}

void CPU::PrintInstruction(bool *data) {
    for (int i = kInstrSizeBit - 1; i >= 0; --i) {
        std::cerr << data[i];
    }
}
