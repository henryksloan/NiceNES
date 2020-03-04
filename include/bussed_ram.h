#pragma once

#include "mirrored_ram.h"

class BussedRAM : public MirroredRAM {
 public:
    virtual inline void write_byte(short addr, uint8_t data) {
        MirroredRAM::write_byte(addr, data);
    }

    virtual inline void write_word(short addr, uint16_t data) {
        MirroredRAM::write_word(addr, data);
    }

    virtual inline uint16_t read_byte(uint16_t addr) {
        return MirroredRAM::read_byte(addr);
    }

    virtual inline uint16_t read_word(uint16_t addr) {
        return MirroredRAM::read_word(addr);
    }

    virtual inline uint8_t &ref_byte(uint16_t addr) {
        return MirroredRAM::ref_byte(addr);
    }

    virtual void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) {
        MirroredRAM::load_file(file, in_start, in_end, mem_start);
    }

    virtual void print() {
        MirroredRAM::print();
    }
};
