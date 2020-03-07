#include "ppu.h"

uint8_t PPU::register_read(uint16_t addr) {
    using namespace PPURegisters;

    switch (addr) {
        case PPUSTATUS: {
            // TODO: Race condition
            uint8_t high3 = ppustatus.high3;
            ppustatus.V = 0;
            back.write_latch = 0;
            return (high3 << 5) | bus_latch.low5;
        }
        case OAMDATA: {
            // TODO: https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
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
            uint8_t temp = ppuctrl;
            ppuctrl = data;
            back.t.N = ppuctrl.N;
            // TODO: NMI on rising edge of V
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
        case OAMDMA: {
            // TODO: Suspend CPU and transfer 0xXX00 to 0xXXFF to OAM
        } break;
        default: {
            // TODO: Handle error
        } break;
    }
}

void PPU::back_fetch() {
    // Tile data is fetched on cycles 1-256
    // Each tile takes 8 cycles: 4 accesses taking 2 cycles each
    switch ((scan.cycle - 1) % 8) {
        case 0: { // Nametable byte
            uint16_t tile_addr = 0x2000 | (back.v & 0x0FFF);
            back.latches.nt = mem->read_byte(tile_addr);
        } break;
        case 2: { // Attribute table byte
            uint16_t attr_addr = 0x23C0
                | (back.v & 0x0C00)
                | ((back.v >> 4) & 0x38)
                | ((back.v >> 2) & 0x07);
            back.latches.attr = mem->read_byte(attr_addr);
            // TODO: Does this already take coarse x and y into account
        } break;
        case 4: { // Pattern table tile low
            uint16_t tile_addr = ppuctrl.B * 0x1000
                + back.latches.nt * 16
                + back.v.y;
            back.latches.tile_lo = mem->read_byte(tile_addr);
        } break;
        case 6: { // Pattern table tile high
            uint16_t tile_addr = ppuctrl.B * 0x1000
                + back.latches.nt * 16
                + back.v.y;
            back.latches.tile_lo = mem->read_byte(tile_addr+8);
        } break;
    }
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
        // TODO: VBlank NMI?
    }
}

void PPU::visible_line() {
    pre_or_visible_cycle();

    if (scan.cycle == 1) clear_oam2();
    if (scan.cycle == 65) sprite_eval();

    // TODO: Draw to framebuf
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
        // TODO: Generate NMI if ppuctrl.V
    }

    scan.cycle++;
    if (scan.cycle > 340) {
        scan.cycle = 0;
        scan.line++;

        if (scan.line > 261) {
            scan.line = 0;
        }
    }
}
