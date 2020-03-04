#pragma once

#include <cstdint>

class Mirror {
 public:
    Mirror(uint16_t start, uint16_t end, uint16_t original_start)
        : start(start), end(end), original_start(original_start) {};
    bool contains(uint16_t addr) { return (addr >= start) && (addr <= end); }
    virtual uint16_t map(uint16_t addr) = 0;

 protected:
    uint16_t start, end, original_start;
};
