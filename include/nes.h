#pragma once

#include "cpu_6502.h"
#include "cartridge.h"
#include "apu.h"
#include "ppu.h"
#include "ram.h"
#include "mirrored_ram.h"
#include "mapped_memory.h"
#include "bussed_ram.h"
#include "mirror.h"

#include <memory>

class NES {
 public:
    NES();

    void load_cartridge(std::shared_ptr<Cartridge> cart);

 private:
    std::unique_ptr<CPU6502> cpu;
    std::unique_ptr<APU> apu;
    std::unique_ptr<PPU> ppu;

    std::shared_ptr<MappedMemory<0x10000>> cpu_mapped_ram;
    std::shared_ptr<MappedMemory<0x4000>> ppu_mapped_ram;
};
