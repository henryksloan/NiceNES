#include "cartridge.h"

bool Cartridge::read_byte(MapperResult target, uint16_t addr, uint8_t &data) {
    uint16_t mapped_addr;
    auto result = mapper->map_read(addr, mapped_addr);
    if (result != target) return false;
    switch(result) {
        case MapperResult::PRG:
            data = prg[mapped_addr];
            break;
        case MapperResult::CHR:
            data = chr[mapped_addr];
            break;
        case MapperResult::NONE:
            break;
    }
    return true;
}

bool Cartridge::write_byte(MapperResult target, uint16_t addr, uint8_t data) {
    uint16_t mapped_addr;
    auto result = mapper->map_write(addr, mapped_addr, data);
    if (result != target) return false;
    switch(result) {
        case MapperResult::PRG:
            prg[mapped_addr] = data;
            break;
        case MapperResult::CHR:
            chr[mapped_addr] = data;
            break;
        case MapperResult::NONE:
            break;
    }
    return true;
}

void Cartridge::reset() {
    if (mapper != nullptr) mapper->reset();
}

Cartridge::MetaData Cartridge::parse_header(std::ifstream &file) {
    struct {
        char NES_label[4];
        uint8_t n_prg_banks;
        uint8_t n_chr_banks;
        uint8_t flags6; // Mapper low, mirroring, battery, trainer
        uint8_t flags7; // Mapper high, VS/Playchoice, NES 2.0
        uint8_t unused[8];
    } header;

    file.read((char*) &header, sizeof(header));

    MirroringMode mirroring = (header.flags6 & 0x1)
        ? MirroringMode::VERTICAL : MirroringMode::HORIZONTAL;
    if (header.flags6 & 0x8) mirroring = MirroringMode::FOURSCREEN;

    return {
        header.n_prg_banks,
        header.n_chr_banks,
        mirroring,
        (bool) (header.flags6 & 0x2), // Battery
        (bool) (header.flags6 & 0x4), // Trainer
        (uint8_t) ((header.flags7 & 0xF0) | (header.flags6 >> 4)), // Mapper
        (bool) (header.flags7 & 0x1), // is_VS
        (bool) (header.flags7 & 0x2) // is_PC10
    };
}

void Cartridge::populate_rom(std::ifstream &file) {
    if (meta.has_trainer) {
        trainer.resize(512);
        file.read((char*) &prg[0], trainer.size());
    }

    prg.resize(16384 * meta.n_prg_banks);
    file.read((char*) &prg[0], prg.size());

    int chr_size = 8192 * ((meta.n_chr_banks == 0) ? 1 : meta.n_chr_banks);
    chr.resize(chr_size);
    file.read((char*) &chr[0], chr.size());

    if (meta.is_PC10) {
        pc_irom.resize(8192);
        file.read((char*) &pc_irom[0], pc_irom.size());
        if (file.is_open()) {
            pc_prom.resize(32);
            file.read((char*) &pc_prom[0], pc_prom.size());
        }
    }
}

template<typename T>
std::shared_ptr<Mapper> Cartridge::make_mapper() {
    return std::make_shared<T>(meta.n_prg_banks, meta.n_chr_banks);
}

void Cartridge::init_mapper() {
    switch (meta.mapper) {
        case   0: mapper = make_mapper<Mapper_000>(); break;
    }
}
