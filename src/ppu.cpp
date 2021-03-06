#include "ppu.h"

uint8_t PPU::register_read(uint16_t addr) {
    using namespace PPURegisters;

    switch (addr) {
        case PPUSTATUS: {
            uint8_t high3 = ppustatus.high3;

            /*
             * "Race condition: Reading PPUSTATUS within two cycles of the start of
             * vertical blank will return 0 in bit 7 but clear the latch anyway,
             * causing NMI to not occur that frame."
             * (https://wiki.nesdev.com/w/index.php/PPU_programmer_reference#Status_.28.242002.29_.3C_read)
             */
            if (scan.line == 241 && scan.cycle == 0)
                high3 &= ~0x80;
            ppustatus.V = 0;
            back.write_latch = 0;
            return (high3 << 5) | bus_latch.low5;
        }
        case OAMDATA: {
            /*
             * Reading OAMDATA during secondary OAM clear returns 0xFF
             * This is because the clear of secondary OAM is done by reading
             * and writing from OAM as usual, but with all reads forced to 0xFF
             * https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation#Details
             */
            bool pre_or_visible_line = scan.line < 240;
            bool oam2_clearing = scan.cycle >= 1 && scan.cycle <= 64;
            if (pre_or_visible_line && oam2_clearing)
                return 0xFF;
            else
                return oam->read_byte(oamaddr);
        }
        case PPUDATA: {
            addr = back.v;
            back.v = back.v + ((ppuctrl.I) ? 32 : 1);
            return mem->read_byte(addr);
        }
        // TODO: Maybe handle OAMDMA seperately as an error
        default:
            // TODO: Handle errors
            return bus_latch;
    }
}

void PPU::register_write(uint16_t addr, uint8_t data) {
    using namespace PPURegisters;

    bus_latch = data;

    switch (addr) {
        case PPUCTRL: {
            auto old_V = ppuctrl.V;
            ppuctrl = data;
            bool toggled_V_on = !old_V && ppuctrl.V;
            if (toggled_V_on && ppustatus.V) {
                cpu_accessor.nmi();
            }
            back.t.N = ppuctrl.N;
        } break;
        case PPUMASK: {
            ppumask = data;
        } break;
        case OAMADDR: {
            oamaddr = data;
        } break;
        case PPUSCROLL: {
            if (back.write_latch == 0) {
                back.t.X = data >> 3;
                x = data & 0x7;
            }
            else {
                back.t.Y = data >> 3;
                back.t.y = data & 0x7;
            }

            back.write_latch = !back.write_latch;
        } break;
        case PPUADDR: {
            if (back.write_latch == 0) {
                back.t.hi = data & 0x3F;
            }
            else {
                back.t.lo = data;
                back.v = back.t;
            }

            back.write_latch = !back.write_latch;
        } break;
        case PPUDATA: {
            mem->write_byte(back.v, data);
            back.v = back.v + ((ppuctrl.I) ? 32 : 1);
        } break;
        case OAMDMA: { // Suspend CPU and transfer 0xXX00 to 0xXXFF to OAM
            cpu_cycle(); // "dummy read cycle while waiting for writes to complete"
            if ((scan.total_cycles / 3) % 2) // "+1 if on an odd CPU cycle"
                cpu_cycle();

            // "256 alternating read/write cycles"
            uint8_t temp;
            for (int i = 0; i < 256; i++) {
                temp = cpu_accessor.read_byte((data << 8) | i);
                cpu_cycle();
                oam->write_byte(i, temp);
                cpu_cycle();
            }
        } break;
        default: {
            // TODO: Handle error
        } break;
    }
}

void PPU::cycle() {
    if (scan.line == 261) { // Pre-render scanline
        prerender_line();
    }
    else if (scan.line < 240) { // Visible scanlines
        visible_line();
    }
    else if (scan.line == 240) { // Post-render scanline
        // Idle
    }
    else if (scan.line == 241 && scan.cycle == 1) { // VBlank tick
        ppustatus.V = 1;
        if (ppuctrl.V) {
            cpu_accessor.nmi();
        }
    }

    scan.total_cycles++;
    scan.cycle++;
    if (scan.cycle > 340) {
        scan.cycle = 0;
        scan.line++;

        if (scan.line > 261) {
            scan.line = 0;
            scan.odd_frame = !scan.odd_frame;
        }
    }
}

void PPU::cpu_cycle() {
    for (int i = 0; i < 3; i++) cycle();
}

void PPU::clear_oam2_byte() {
    if (scan.cycle % 2 == 1) {
        oam2->write_byte((scan.cycle - 1) / 2, 0xFF);
    }
}

void PPU::back_fetch() {
    // Tile data is fetched on cycles 1-256
    // Each tile takes 8 cycles: 4 accesses taking 2 cycles each
    switch ((scan.cycle - 1) % 8) {
        case 0: { // Feed from latches to shift registers
            /*
             * Every 8 cycles, the top byte of the each of the two
             * 16-bit tile shift registers are filled by the latches.
             * These registers correspond to a tile's high and low pattern table bytes
             */
            back.tile[0] &= 0xFF00;
            back.tile[0] |= back.latches.tile_lo;
            back.tile[1] &= 0xFF00;
            back.tile[1] |= back.latches.tile_hi;

            // Likewise, these two bits are the high and low attribute bytes
            // This latch is later shifted into the attribute shift registers `attr`
            back.attr_latch[0] = back.latches.attr & 1;
            back.attr_latch[1] = back.latches.attr & 2;
        } break;
        case 1: { // Nametable byte
            uint16_t tile_addr = 0x2000 | (back.v & 0x0FFF);
            back.latches.nt = mem->read_byte(tile_addr);
        } break;
        case 3: { // Attribute table byte
            uint16_t attr_addr = 0x23C0
                | (back.v & 0x0C00)
                | ((back.v >> 4) & 0x38)
                | ((back.v >> 2) & 0x07);
            back.latches.attr = mem->read_byte(attr_addr);
            // TODO: Check this
            if (back.v.Y & 2) back.latches.attr >>= 4;
            if (back.v.X & 2) back.latches.attr >>= 2;
        } break;
        case 5: { // Pattern table tile low
            uint16_t tile_addr = ppuctrl.B * 0x1000
                + back.latches.nt * 16
                + back.v.y;
            back.latches.tile_lo = mem->read_byte(tile_addr);
        } break;
        case 7: { // Pattern table tile high
            uint16_t tile_addr = ppuctrl.B * 0x1000
                + back.latches.nt * 16
                + back.v.y;
            back.latches.tile_hi = mem->read_byte(tile_addr+8);
        } break;
    }
}

void PPU::sprite_fetch() {
    // Sprite data is fetched on cycles 257-320
    // Each sprite tile takes 8 cycles: 4 accesses taking 2 cycles each
    switch ((scan.cycle - 1) % 8) {
        case 0: { // Garbage nametable byte
            // TODO
        } break;
        case 2: { // Garbage nametable byte
            // TODO
        } break;
        case 4: { // Pattern table tile low
            // TODO
        } break;
        case 6: { // Pattern table tile high
            // TODO
        } break;
    }
}

void PPU::sprite_eval() {
    // TODO: https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
}

void PPU::pre_or_visible_cycle() {
    if (scan.cycle == 0) {
        // Idle cycle
        return;
    }
    else if (scan.cycle >= 1 && scan.cycle <= 256) {
        back_fetch();
    }
    else if (scan.cycle >= 257 && scan.cycle <= 320) {
        sprite_fetch();
    }
    else if((scan.cycle >= 321 && scan.cycle <= 336)) {
        back_fetch(); // For next line
    }
    else if((scan.cycle >= 337 && scan.cycle <= 340)) {
        // "Two bytes are fetched, but the purpose for this is unknown" (Nesdev)
        // "At least one mapper -- MMC5 -- is known to use this... to clock a scanline counter"
        if (this->scan.cycle % 2 == 0) {
            back.latches.nt = mem->read_byte(0x2000 | (back.v & 0x0FFF));
        }
    }
}

void PPU::prerender_line() {
    // Last cycle of pre-render line is skipped on odd frames
    if (scan.cycle == 340 && scan.odd_frame) return;

    pre_or_visible_cycle();

    if (scan.cycle == 1) {
        ppustatus.V = 0;
        ppustatus.S = 0;
        ppustatus.O = 0;
    }
}

void PPU::visible_line() {
    pre_or_visible_cycle();

    if (scan.cycle >= 1 && scan.cycle <= 64) {
        clear_oam2_byte();
    }
    else if (scan.cycle >= 65 && scan.cycle <= 256) {
        sprite_eval();
    }

    // TODO: Draw to framebuf
}
