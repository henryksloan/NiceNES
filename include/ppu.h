#pragma once

#include "memory.h"

#include <cstdint>
#include <memory>

class PPU {
 public:
    PPU(std::shared_ptr<Memory> mem)
        : mem(mem) {};

    uint8_t register_read(uint16_t addr);
    uint8_t &register_ref(uint16_t addr);
    void register_write(uint16_t addr, uint8_t data);

 private:
    std::shared_ptr<Memory> mem;
};
