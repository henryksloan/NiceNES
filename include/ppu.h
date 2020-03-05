#pragma once

#include <memory>

class PPU {
 public:
    PPU();

    uint8_t register_read(uint16_t addr);
    uint8_t &register_ref(uint16_t addr);
    void register_write(uint16_t addr, uint8_t data);
};
