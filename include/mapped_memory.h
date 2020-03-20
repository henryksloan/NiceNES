#pragma once

#include "memory.h"
#include "cartridge.h"

// Wraps a Memory instance with a mapper
template<size_t SIZE>
class MappedMemory : public Memory {
 public:
    MappedMemory(std::unique_ptr<Memory> memory, MapperResult target)
        : memory(std::move(memory)), cart(nullptr), target(target) {};

    virtual void write_byte(uint16_t addr, uint8_t data) {
        if (target == MapperResult::CHR) addr = nametable_mirror(addr);

        if (cart && cart->write_byte(target, addr, data)) {}
        else memory->write_byte(addr, data);
    }

    virtual void write_word(uint16_t addr, uint16_t data) {
        write_byte(addr, data & 0x00FF);
        write_byte(addr+1, (data & 0xFF00) >> 8);
    }

    virtual uint8_t read_byte(uint16_t addr) {
        if (target == MapperResult::CHR) addr = nametable_mirror(addr);

        uint8_t out;
        if (cart && cart->read_byte(target, addr, out)) return out;
        else return memory->read_byte(addr);
    }

    virtual uint16_t read_word(uint16_t addr) {
        uint8_t lo = read_byte(addr);
        uint8_t hi = read_byte(addr+1);
        return (hi << 8) | lo;
    }

    virtual inline uint8_t &ref_byte(uint16_t addr) {
        if (target == MapperResult::CHR) addr = nametable_mirror(addr);
        return memory->ref_byte(addr);
    }

    virtual void ref_callback(uint16_t addr) {
        if (target == MapperResult::CHR) addr = nametable_mirror(addr);
        memory->ref_callback(addr);
    }

    virtual void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) {
        memory->load_file(file, in_start, in_end, mem_start);
    }

    virtual void print() {
        memory->print();
    }

    void load_cartridge(std::shared_ptr<Cartridge> cart) {
        this->cart = cart;
    }

    uint16_t nametable_mirror(uint16_t addr) {
        if (!cart) return addr;

        MirroringMode mode = cart->meta.mirroring;
        switch (mode) {
            case MirroringMode::HORIZONTAL:
                if ((addr >= 0x2400 && addr <= 0x27FF)
                 || (addr >= 0x2C00 && addr <= 0x2FFF)) return addr - 0x400;
                return addr;
            case MirroringMode::VERTICAL:
                if ((addr >= 0x2800 && addr <= 0x2BFF)
                 || (addr >= 0x2C00 && addr <= 0x2FFF)) return addr - 0x800;
                return addr;
            case MirroringMode::FOURSCREEN:
                return addr; // TODO
            case MirroringMode::SINGLESCREEN_LO:
                return addr; // TODO
            case MirroringMode::SINGLESCREEN_HI:
                return addr; // TODO
            default:
                return addr;
        }
    }

 private:
    std::unique_ptr<Memory> memory;
    std::shared_ptr<Cartridge> cart;
    MapperResult target;
};

template class MappedMemory<0x4000>;
template class MappedMemory<0x10000>;
