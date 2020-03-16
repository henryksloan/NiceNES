#include "cartridge.h"

Cartridge::MetaData Cartridge::populate_rom(std::ifstream &file) {
    struct {
        char NES_label[4];
        uint8_t n_prg_banks;
        uint8_t n_chr_banks;
        uint8_t flags6; // Mapper low, mirroring, battery, trainer
        uint8_t flags7; // Mapper high, VS/Playchoice, NES 2.0
        uint8_t unused;
    } header;

    file.read((char*) &header, sizeof(header));

    MirroringMode mirroring = (header.flags6 & 0x1)
        ? MirroringMode::VERTICAL : MirroringMode::HORIZONTAL;
    if (header.flags6 & 0x8) mirroring = MirroringMode::FOURSCREEN;

    bool has_trainer = (bool) (header.flags6 & 0x4);
    if (has_trainer) {
        trainer.resize(512);
        file.read((char*) trainer.data(), trainer.size());
    }

    prg.resize(16384 * header.n_prg_banks);
    file.read((char*) prg.data(), prg.size());

    int chr_size = 8192 * ((header.n_chr_banks == 0) ? 1 : header.n_chr_banks);
    chr.resize(chr_size);
    file.read((char*) chr.data(), chr.size());

    bool is_PC10 = (bool) (header.flags7 & 0x2);
    /* if (is_PC10) {
        pc_irom.resize(8192);
        file.read((char*) pc_irom.data(), pc_irom.size());
        pc_prom.resize(32); // TODO: This is often missing; make sure it's checked
        file.read((char*) pc_prom.data(), pc_prom.size());
    } */

    return {
        header.n_prg_banks,
        header.n_chr_banks,
        mirroring,
        (bool) (header.flags6 & 0x2), // Battery
        has_trainer,
        (uint8_t) ((header.flags7 & 0xF0) | (header.flags6 >> 4)), // Mapper
        (bool) (header.flags7 & 0x1), // is_VS
        is_PC10
    };
}
