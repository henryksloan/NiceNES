#include "mirrored_ram.h"

uint16_t MirroredRAM::map(uint16_t addr) {
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
