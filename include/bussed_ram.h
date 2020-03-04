#pragma once

#include "mirrored_ram.h"
#include "mapped_registers.h"

#include <vector>

class BussedRAM : public MirroredRAM {
 public:
    BussedRAM(std::vector<std::unique_ptr<Mirror>> mirrors)
        : MirroredRAM(mirrors) {};

    virtual void write_byte(short addr, uint8_t data);
    virtual void write_word(short addr, uint16_t data);
    virtual uint16_t read_byte(uint16_t addr);
    virtual inline uint16_t read_word(uint16_t addr);
    virtual inline uint8_t &ref_byte(uint16_t addr);
    virtual void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start);
    virtual void print();

 private:
    std::vector<std::unique_ptr<MappedRegisters>> mapped_registers;

    std::vector<std::unique_ptr<MappedRegisters>>::iterator get_mapped_registers(uint16_t addr) {
        return std::find_if(mapped_registers.begin(), mapped_registers.end(),
            [&](std::unique_ptr<MappedRegisters> &reg) { return reg->contains(addr); });
    };
};
