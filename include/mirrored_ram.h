#pragma once

#include "ram.h"
#include "mirror.h"

#include <vector>
#include <utility>
#include <algorithm>
#include <memory>

class MirroredRAM : public RAM {
 public:
    MirroredRAM(std::vector<std::unique_ptr<Mirror>> mirrors)
        : mirrors(mirrors) {};

    virtual inline void write_byte(short addr, uint8_t data) {
        RAM::write_byte(map(addr), data);
    }

    virtual inline void write_word(short addr, uint16_t data) {
        RAM::write_word(map(addr), data);
    }

    virtual inline uint16_t read_byte(uint16_t addr) {
        return RAM::read_byte(map(addr));
    }

    virtual inline uint16_t read_word(uint16_t addr) {
        return RAM::read_word(map(addr));
    }

    virtual inline uint8_t &ref_byte(uint16_t addr) {
        return RAM::ref_byte(map(addr));
    }

    virtual void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) {
        RAM::load_file(file, in_start, in_end, mem_start);
    }

    virtual void print() {
        RAM::print();
    }

 protected:
    uint16_t map(uint16_t addr);

 private:
    std::vector<std::unique_ptr<Mirror>> mirrors;
};
