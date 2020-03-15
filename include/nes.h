#pragma once

#include "cpu_6502.h"
#include "apu.h"
#include "ppu.h"
#include "ram.h"
#include "mirrored_ram.h"
#include "bussed_ram.h"
#include "mirror.h"

#include <memory>

class NES {
 public:
    NES();

 private:
    std::unique_ptr<CPU6502> cpu;
    std::unique_ptr<APU> apu;
    std::unique_ptr<PPU> ppu;
};
