#pragma once

#include "memory.h"
#include "register.h"
#include "bit_mask.h"
#include "cpu_accessor.h"

#include <cstdint>
#include <memory>
#include <functional>

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
    PPU(std::shared_ptr<Memory> mem,
        std::shared_ptr<Memory> oam,
        std::shared_ptr<Memory> oam2,
        const CPUAccessor &cpu_accessor)
        : mem(mem), oam(oam), oam2(oam2), cpu_accessor(cpu_accessor) {};

    uint8_t register_read(uint16_t addr);
    void register_write(uint16_t addr, uint8_t data);

    void cycle();
    void cpu_cycle(); // Run the number of PPU cycles per one CPU cycle (NTSC: 3)

 private:
    std::shared_ptr<Memory> mem;
    std::shared_ptr<Memory> oam;
    std::shared_ptr<Memory> oam2;

    const CPUAccessor &cpu_accessor;

    struct /* ppuctrl */ : Register {
        BitMask V{raw, 0x80};
        BitMask P{raw, 0x40};
        BitMask H{raw, 0x20};
        BitMask B{raw, 0x10};
        BitMask S{raw, 0x08};
        BitMask I{raw, 0x04};
        BitMask N{raw, 0x03};
        using Register::operator uint8_t;
        using Register::operator=;
    } ppuctrl;

    struct /* ppumask */ : Register {
        BitMask B{raw, 0x80};
        BitMask G{raw, 0x40};
        BitMask R{raw, 0x20};
        BitMask s{raw, 0x10};
        BitMask b{raw, 0x08};
        BitMask M{raw, 0x04};
        BitMask m{raw, 0x02};
        BitMask g{raw, 0x01};
        using Register::operator uint8_t;
        using Register::operator=;
    } ppumask;

    struct /* ppustatus */ : Register {
        BitMask high3{raw, 0xE0};
        BitMask V{raw, 0x80};
        BitMask S{raw, 0x40};
        BitMask O{raw, 0x20};
        // Last 5 bytes are junk, i.e. read from bus latch
        using Register::operator uint8_t;
        using Register::operator=;
    } ppustatus;

    uint8_t oamaddr;
    uint8_t oamdata;

    uint8_t x;

    struct /* bus_latch */ : Register {
        BitMask low5{raw, 0x1F};
        using RegisterTemplate::operator uint8_t;
        using RegisterTemplate::operator=;
    } bus_latch;

    struct /* back */ {
        struct /* v, t */ : Register16 {
            BitMask y{raw, 0x7000};
            BitMask N{raw, 0x0C00};
            BitMask Y{raw, 0x03E0};
            BitMask X{raw, 0x001F};
            using RegisterTemplate::operator uint16_t;
            using RegisterTemplate::operator=;
        } v, t;
        bool write_latch;

        // Temporary latches for background fetches
        struct /* latches */ {
            uint8_t nt;
            uint8_t attr;
            uint8_t tile_lo;
            uint8_t tile_hi;
        } latches;

        uint16_t tile[2];

        uint8_t attr[2];
        bool attr_latch[2];
    } back;

    struct /* sprites */ {
        Register16 patt[8];
        uint8_t attr[8];
        uint8_t xpos[8];
    } sprites;

    struct /* scan */ {
        unsigned line;
        unsigned cycle;
        unsigned total_cycles;
        bool odd_frame;
    } scan;

    void clear_oam2();
    void back_fetch();
    void sprite_fetch();
    void sprite_eval();
    void pre_or_visible_cycle();

    void prerender_line();
    void visible_line();
};
