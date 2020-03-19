#pragma once

#include <cstdint>

enum class MapperResult {
    PRG,
    CHR,
    NONE
};

class Mapper {
 public:
    Mapper(int n_prg_banks, int n_chr_banks)
        : n_prg_banks(n_prg_banks), n_chr_banks(n_chr_banks) {};

    virtual MapperResult map_read(uint16_t addr, uint16_t &mapped_addr) = 0;
    virtual MapperResult map_write(uint16_t addr, uint16_t &mapped_addr, uint8_t data) = 0;
    virtual void reset() = 0;

 protected:
    int n_prg_banks;
    int n_chr_banks;
};
