#include "repeat_map.h"

uint16_t RepeatMirror::map(uint16_t addr) {
    if ((addr >= start) && (addr <= end)) {
        return original_start + ((addr-start)%len);
    }
    else {
        return addr;
    }
}
