#include "mapper_000.h"

Mapper_000::Mapper_000(int n_prg_banks, int n_chr_banks)
    : Mapper(n_prg_banks, n_chr_banks) {}

MapperResult Mapper_000::map_read(uint16_t addr, uint16_t &mapped_addr) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped_addr = addr;
        return MapperResult::CHR;
    }
    else if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (n_prg_banks > 1 ? 0x7FFF : 0x3FFF);
        return MapperResult::PRG;
    }

    return MapperResult::NONE;
}

MapperResult Mapper_000::map_write(uint16_t addr, uint16_t &mapped_addr, uint8_t data) {
    return map_read(addr, mapped_addr);
}

void Mapper_000::reset() {};
