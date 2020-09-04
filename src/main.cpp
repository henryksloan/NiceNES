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
    // auto cart = std::make_shared<Cartridge>(file);
    // nes.set_cart(cart);
    nes.set_cart(file);
    // for (int i = 0; i < 256; i++) {
    //     for (int j = 0; j < 256; j++) {
    //         std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) nes.main_ram->read_byte(0xC000 + i*256 + j) << " ";
    //     }
    //     std::cout << std::endl;
    // }
    for (int i = 0; i < 0x3FFF; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) nes.main_ram->read_byte(0xC000 + i) << " ";
    }
    std::cout << std::endl;
    // for (int i = 0; i < 32; i++) nes.step();
    while (true) nes.step();

    return 0;
}
