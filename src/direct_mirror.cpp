#include "direct_mirror.h"

uint16_t DirectMirror::map(uint16_t addr) {
    if ((addr >= start) && (addr <= end)) {
        return original_start + (addr-start);
    }
    else {
        return addr;
    }
}
