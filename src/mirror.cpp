#include "mirror.h"

bool Mirror::contains(uint16_t addr) {
    return (addr >= start) && (addr <= end);
}

uint16_t Mirror::map(uint16_t addr) {
    if ((addr >= start) && (addr <= end)) {
        return original_start + ((addr-start) % (original_end-original_start));
    }
    else {
        return addr;
    }
}
