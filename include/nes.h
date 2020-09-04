#pragma once

#include "cpu_6502.h"
#include "apu.h"
#include "ppu.h"
#include "ram.h"
#include "cartridge.h"
#include "mirrored_ram.h"
#include "bussed_ram.h"
#include "mirror.h"

#include <memory>

class NES {
 public:
    NES();
    // void set_cart(std::shared_ptr<Cartridge> cart) { this->cart = cart; };
    void set_cart(std::ifstream &file) { this->cart->reload(file); };
    void step() { cpu->step(); }
    std::shared_ptr<BussedRAM<0x10000>> main_ram;

 private:
    std::unique_ptr<CPU6502> cpu;
    std::unique_ptr<APU> apu;
    std::unique_ptr<PPU> ppu;

    std::shared_ptr<Cartridge> cart;
};
