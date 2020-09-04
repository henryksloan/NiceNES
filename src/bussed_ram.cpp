#include "bussed_ram.h"

template<size_t SIZE>
void BussedRAM<SIZE>::write_byte(uint16_t addr, uint8_t data) {
    // TODO: Uncomment
    // const auto &reg = get_mapped_registers(addr);
    // if (reg != mapped_registers.end()) {
    //     (*reg)->write(addr, data);
    // }
    if (addr == 0x00) {
        std::cout << "Write 0x00 := " << (int) data << std::endl;
    }
    MirroredRAM<SIZE>::write_byte(addr, data);
}

template<size_t SIZE>
void BussedRAM<SIZE>::write_word(uint16_t addr, uint16_t data) {
    // TODO: Fix edge case behavior of word write to mapped registers
    // Should probably just treat it as two seperate writes
    MirroredRAM<SIZE>::write_word(addr, data);
}

template<size_t SIZE>
uint8_t BussedRAM<SIZE>::read_byte(uint16_t addr) {
    uint8_t data = 0;
    if (cart && cart->read_byte(addr, data)) {
        return data;
    }

    auto reg = get_mapped_registers(addr);
    if (reg != mapped_registers.end()) {
        return (*reg)->read(addr);
    }
    return MirroredRAM<SIZE>::read_byte(addr);
}

template<size_t SIZE>
uint16_t BussedRAM<SIZE>::read_word(uint16_t addr) {
    return read_byte(addr) + (read_byte(addr+1) << 8);
    // TODO: Fix edge case behavior of word read of mapped registers
    // Should probably just treat it as two seperate writes
    // return MirroredRAM<SIZE>::read_word(addr);
}

template<size_t SIZE>
uint8_t &BussedRAM<SIZE>::ref_byte(uint16_t addr) {
    uint8_t data = 0;
    if (cart && cart->read_byte(addr, data)) {
        return cart->ref_byte(addr);
    }

    return MirroredRAM<SIZE>::ref_byte(addr);
    // TODO: This may be a problem, but it should work since ref_callback will end up calling write_word
    // This makes sense given the semantics of ref_callback
    // auto reg = get_mapped_registers(addr);
    // if (reg != mapped_registers.end()) {
    //     return (*reg)->ref(addr);
    // }
    // else {
    // return MirroredRAM<SIZE>::ref_byte(addr);
    // }
}

template<size_t SIZE>
void BussedRAM<SIZE>::ref_callback(uint8_t &data) {
    std::cout << "YOYO: " << (int) data << std::endl;
    // MirroredRAM<SIZE>::ref_callback(data);
    auto mem_p = MirroredRAM<SIZE>::mem.data();
    auto mem_len = MirroredRAM<SIZE>::mem.size();
    if ((&data >= mem_p) && (&data < (mem_p+mem_len))) {
        write_byte((&data)-mem_p, data);
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
