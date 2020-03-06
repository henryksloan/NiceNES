#include "bussed_ram.h"

template<size_t SIZE>
void BussedRAM<SIZE>::write_byte(short addr, uint8_t data) {
    const auto &reg = get_mapped_registers(addr);
    if (reg != mapped_registers.end()) {
        (*reg)->write(addr, data);
    }
    else {
        MirroredRAM<SIZE>::write_byte(addr, data);
    }
}

template<size_t SIZE>
void BussedRAM<SIZE>::write_word(short addr, uint16_t data) {
    // TODO: Fix edge case behavior of word write to mapped registers
    // Should probably just treat it as two seperate writes
    MirroredRAM<SIZE>::write_word(addr, data);
}

template<size_t SIZE>
uint16_t BussedRAM<SIZE>::read_byte(uint16_t addr) {
    auto reg = get_mapped_registers(addr);
    if (reg != mapped_registers.end()) {
        return (*reg)->read(addr);
    }
    else {
        return MirroredRAM<SIZE>::read_byte(addr);
    }
}

template<size_t SIZE>
uint16_t BussedRAM<SIZE>::read_word(uint16_t addr) {
    // TODO: Fix edge case behavior of word read of mapped registers
    // Should probably just treat it as two seperate writes
    return MirroredRAM<SIZE>::read_word(addr);
}

template<size_t SIZE>
uint8_t &BussedRAM<SIZE>::ref_byte(uint16_t addr) {
    auto reg = get_mapped_registers(addr);
    if (reg != mapped_registers.end()) {
        return (*reg)->ref(addr);
    }
    else {
        return MirroredRAM<SIZE>::ref_byte(addr);
    }
}

template<size_t SIZE>
void BussedRAM<SIZE>::load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) {
    MirroredRAM<SIZE>::load_file(file, in_start, in_end, mem_start);
}

template<size_t SIZE>
void BussedRAM<SIZE>::print() {
    MirroredRAM<SIZE>::print();
}

template class BussedRAM<0x10000>;