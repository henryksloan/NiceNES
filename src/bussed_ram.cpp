#include "bussed_ram.h"

void BussedRAM::write_byte(short addr, uint8_t data) {
    auto reg = get_mapped_registers(addr);
    if (reg != mapped_registers.end()) {
        (*reg)->write(addr, data);
    }
    else {
        MirroredRAM::write_byte(addr, data);
    }
}

void BussedRAM::write_word(short addr, uint16_t data) {
    // TODO: Fix edge case behavior of word write to mapped registers
    // Should probably just treat it as two seperate writes
    MirroredRAM::write_word(addr, data);
}

uint16_t BussedRAM::read_byte(uint16_t addr) {
    auto reg = get_mapped_registers(addr);
    if (reg != mapped_registers.end()) {
        return (*reg)->read(addr);
    }
    else {
        return MirroredRAM::read_byte(addr);
    }
}

uint16_t BussedRAM::read_word(uint16_t addr) {
    // TODO: Fix edge case behavior of word read of mapped registers
    // Should probably just treat it as two seperate writes
    return MirroredRAM::read_word(addr);
}

uint8_t &BussedRAM::ref_byte(uint16_t addr) {
    auto reg = get_mapped_registers(addr);
    if (reg != mapped_registers.end()) {
        return (*reg)->ref(addr);
    }
    else {
        return MirroredRAM::ref_byte(addr);
    }
}

void BussedRAM::load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) {
    MirroredRAM::load_file(file, in_start, in_end, mem_start);
}

void BussedRAM::print() {
    MirroredRAM::print();
}
