#include "ppu.h"

uint8_t PPU::register_read(uint16_t addr) {
    using namespace PPURegisters;

    switch (addr) {
        case PPUSTATUS: {
            // TODO: Race condition
            uint8_t high3 = ppustatus.high3;
            ppustatus.V = 0;
            write_toggle = 0;
            return (high3 << 5) | bus_latch.low5;
        }
        case OAMDATA: {
            // TODO: https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
            return oam->read_byte(oamaddr);
        }
        case PPUDATA: {
            addr = v;
            v = v + ((ppuctrl.I) ? 32 : 1);
            return mem->read_byte(addr);
        }
        // TODO: Maybe handle OAMDMA seperately as an error
        default:
            // TODO: Handle errors
            return bus_latch;
    }
}

// TODO: This may not be necassary at all
// ref_byte will always correspond with a ref_callback, thus calling write_byte
uint8_t &PPU::register_ref(uint16_t addr) {
    uint8_t x = 5;
    return x;
}

void PPU::register_write(uint16_t addr, uint8_t data) {
    using namespace PPURegisters;

    bus_latch = data;

    switch (addr) {
        case PPUCTRL: {
            uint8_t temp = ppuctrl;
            ppuctrl = data;
            t.N = ppuctrl.N;
            // TODO: NMI on rising edge of V
            break;
        }
        case PPUMASK:
            ppumask = data;
            break;
        case OAMADDR:
            oamaddr = data;
            break;
        case PPUSCROLL: {
            if (write_toggle == 0) {
                t.X = data >> 3;
                x = data & 0x7;
            }
            else {
                t.Y = data >> 3;
                t.y = data & 0x7;
            }

            write_toggle = !write_toggle;
            break;
        }
        case PPUADDR: {
            if (write_toggle == 0) {
                t.hi = data & 0x3F;
            }
            else {
                t.lo = data;
                v = t;
            }

            write_toggle = !write_toggle;
            break;
        }
        case PPUDATA: {
            mem->write_byte(v, data);
            v = v + ((ppuctrl.I) ? 32 : 1);
            break;
        }
        case OAMDMA: {
            // TODO: Suspend CPU and transfer 0xXX00 to 0xXXFF to OAM
            break;
        }
        default: {
            // TODO: Handle error
            break;
        }
    }
}
