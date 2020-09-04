#pragma once

#include <fstream>
#include <vector>

#include <iterator>
#include <iostream>

enum class MirroringMode {
    HORIZONTAL,
    VERTICAL,
    FOURSCREEN,
    SINGLESCREEN_LO,
    SINGLESCREEN_HI
};

class Cartridge {
 public:
    Cartridge() {}

    Cartridge(std::ifstream &file)
        : meta(parse_header(file)) {
        populate_rom(file);
    }

    void reload(std::ifstream &file) {
        meta = parse_header(file);
        populate_rom(file);
    }

    // const struct MetaData {
    struct MetaData {
        int n_prg_banks;
        int n_chr_banks;
        MirroringMode mirroring;
        bool has_battery;
        bool has_trainer;
        uint8_t mapper;
        bool is_VS;
        bool is_PC10;
    } meta;

    bool read_byte(uint16_t addr, uint8_t &data) {
        if (addr >= 0xC000 && addr <= 0xFFFF && prg.size() > 0) {
            data = prg[(addr - 0xC000) % prg.size()];
            return true;
        }
        return false;
    }

    uint8_t &ref_byte(uint16_t addr) {
        return prg.at((addr - 0xC000) % prg.size());
    }

 private:
    // parse_header populates meta, leaving file iterator right after the header
    MetaData parse_header(std::ifstream &file);
    void populate_rom(std::ifstream &file);

    std::vector<uint8_t> prg; // Program ROM
    std::vector<uint8_t> chr; // Character ROM

    // Usually empty ROM sectors
    std::vector<uint8_t> trainer; // Trainer ROM
    std::vector<uint8_t> pc_irom; // PlayChoice INST-ROM
    std::vector<uint8_t> pc_prom; // PlayChoice ROM
};
