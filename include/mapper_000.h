#pragma once

#include "mapper.h"

#include <cstdint>

class Mapper_000 : public Mapper {
 public:
    Mapper_000(int n_prg_banks, int n_chr_banks);

    MapperResult map_read(uint16_t addr, uint16_t &mapped_addr) override;
    MapperResult map_write(uint16_t addr, uint16_t &mapped_addr, uint8_t data) override;
    void reset() override;
};
