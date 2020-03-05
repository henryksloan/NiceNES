#include "mirrored_ram.h"

template <size_t SIZE>
uint16_t MirroredRAM<SIZE>::map(uint16_t addr) {
    // TODO: Consider/benchmark whether to just apply all mirrors
    auto mirror = std::find_if(mirrors.begin(), mirrors.end(),
            [addr](std::unique_ptr<Mirror> mirror) { return mirror->contains(addr); });
    if (mirror == mirrors.end()) {
        return addr;
    }
    else {
        return (*mirror)->map(addr);
    }
}
