#pragma once

#include "mirrored_ram.h"
#include "mapped_registers.h"
#include "cartridge.h"

#include <vector>

template<size_t SIZE>
class BussedRAM : public MirroredRAM<SIZE> {
 public:
    BussedRAM(std::vector<std::unique_ptr<Mirror>> mirrors,
        std::vector<std::unique_ptr<MappedRegisters>> mapped_registers,
        const std::shared_ptr<Cartridge> cart)
        : MirroredRAM<SIZE>(std::move(mirrors)), mapped_registers(std::move(mapped_registers)), cart(cart) {};

    virtual void write_byte(uint16_t addr, uint8_t data);
    virtual void write_word(uint16_t addr, uint16_t data);
    virtual uint8_t read_byte(uint16_t addr);
    virtual uint16_t read_word(uint16_t addr);
    virtual uint8_t &ref_byte(uint16_t addr);
    virtual void ref_callback(uint8_t &data);
    virtual void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start);
    virtual void print();

 private:
    const std::vector<std::unique_ptr<MappedRegisters>> mapped_registers;
    const std::shared_ptr<Cartridge> cart;

    std::vector<std::unique_ptr<MappedRegisters>>::const_iterator get_mapped_registers(uint16_t addr) {
        return std::find_if(mapped_registers.begin(), mapped_registers.end(),
            [&](const std::unique_ptr<MappedRegisters> &reg) { return reg->contains(addr); });
    };
};
