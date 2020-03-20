#pragma once

#include "mirroring_mode.h"

#include "mapper.h"
#include "mapper_000.h"

#include <fstream>
#include <memory>
#include <vector>

class Cartridge {
 public:
    Cartridge(std::ifstream &file)
        : meta(parse_header(file)) {
        populate_rom(file);
        init_mapper();
    }

    bool read_byte(MapperResult target, uint16_t addr, uint8_t &data);
    bool write_byte(MapperResult target, uint16_t addr, uint8_t data);
    void reset();

    const struct MetaData {
        int n_prg_banks;
        int n_chr_banks;
        MirroringMode mirroring;
        bool has_battery;
        bool has_trainer;
        uint8_t mapper;
        bool is_VS;
        bool is_PC10;
    } meta;

 private:
    // parse_header populates meta, leaving file iterator right after the header
    MetaData parse_header(std::ifstream &file);
    void populate_rom(std::ifstream &file);

    // Helper function to create a mapper with appropriate bank counts
    template<typename T>
    std::shared_ptr<Mapper> make_mapper();

    // Initializes mapper from meta.mapper
    void init_mapper();

    std::shared_ptr<Mapper> mapper;

    std::vector<uint8_t> prg; // Program ROM
    std::vector<uint8_t> chr; // Character ROM

    // Usually empty ROM sectors
    std::vector<uint8_t> trainer; // Trainer ROM
    std::vector<uint8_t> pc_irom; // PlayChoice INST-ROM
    std::vector<uint8_t> pc_prom; // PlayChoice ROM
};
