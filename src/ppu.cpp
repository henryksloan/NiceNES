#include "ppu.h"

uint8_t PPU::register_read(uint16_t addr) {
    return 0;
}

uint8_t &PPU::register_ref(uint16_t addr) {
    uint8_t x = 5;
    return x;
}

void PPU::register_write(uint16_t addr, uint8_t data) {
}
