#include "mirrored_ram.h"

template <size_t SIZE>
uint16_t MirroredRAM<SIZE>::map(uint16_t addr) {
    // TODO: Consider/benchmark whether to just apply all mirrors
    const auto &mirror = std::find_if(mirrors.begin(), mirrors.end(),
            [addr](const std::unique_ptr<Mirror> &mirror) { return mirror->contains(addr); });
    if (mirror == mirrors.end()) {
        return addr;
    }
    else {
        return (*mirror)->map(addr);
    }
}

template class MirroredRAM<0x4000>;
template class MirroredRAM<0x10000>;
