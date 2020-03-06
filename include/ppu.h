#pragma once

#include "memory.h"
#include "register.h"
#include "bit_mask.h"

#include <cstdint>
#include <memory>

// From https://github.com/daniel5151/ANESE
namespace PPURegisters {
    enum Reg : uint16_t {
        PPUCTRL   = 0x2000,
        PPUMASK   = 0x2001,
        PPUSTATUS = 0x2002,
        OAMADDR   = 0x2003,
        OAMDATA   = 0x2004,
        PPUSCROLL = 0x2005,
        PPUADDR   = 0x2006,
        PPUDATA   = 0x2007,
        OAMDMA    = 0x4014
    };
};

class PPU {
 public:
    PPU(std::shared_ptr<Memory> mem)
        : mem(mem) {};

    uint8_t register_read(uint16_t addr);
    uint8_t &register_ref(uint16_t addr);
    void register_write(uint16_t addr, uint8_t data);

 private:
    std::shared_ptr<Memory> mem;

    struct /* ppuctrl */ : Register {
        const BitMask V{raw, 0x80};
        const BitMask P{raw, 0x40};
        const BitMask H{raw, 0x20};
        const BitMask B{raw, 0x10};
        const BitMask S{raw, 0x08};
        const BitMask I{raw, 0x04};
        const BitMask N{raw, 0x03};
        using Register::operator uint8_t;
        using Register::operator=;
    } ppuctrl;

    struct /* ppumask */ : Register {
        const BitMask B{raw, 0x80};
        const BitMask G{raw, 0x40};
        const BitMask R{raw, 0x20};
        const BitMask s{raw, 0x10};
        const BitMask b{raw, 0x08};
        const BitMask M{raw, 0x04};
        const BitMask m{raw, 0x02};
        const BitMask g{raw, 0x01};
        using Register::operator uint8_t;
        using Register::operator=;
    } ppumask;

    struct /* ppustatus */ : Register {
        const BitMask V{raw, 0x80};
        const BitMask S{raw, 0x40};
        const BitMask O{raw, 0x20};
        const BitMask etc{raw, 0x1F};
        using Register::operator uint8_t;
        using Register::operator=;
    } ppustatus;

    uint8_t oamaddr;
    uint8_t oamdata;

    struct /* v, t */ : Register16 {
        BitMask y{raw, 0x7000};
        BitMask N{raw, 0x0C00};
        BitMask Y{raw, 0x03E0};
        BitMask X{raw, 0x001F};
        using RegisterTemplate::operator uint16_t;
        using RegisterTemplate::operator=;
    } v, t;

    uint8_t x;
    uint8_t write_toggle;
};
