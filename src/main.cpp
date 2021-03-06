#include "bussed_ram.h"
#include "ppu.h"
#include "cpu_6502.h"
#include "nes.h"
#include "cartridge.h"

#include <iostream>
#include <memory>
#include <fstream>

int main(int argc, char **argv) {
    std::cout << "Welcome to NiceNES!" << std::endl;
    NES nes;

    std::ifstream file(argv[1], std::ifstream::binary);
    if (!file) {
        std::cerr << "Could not read file " << argv[1] << std::endl;
        return 1;
    }
    Cartridge cart(file);

    return 0;
}
