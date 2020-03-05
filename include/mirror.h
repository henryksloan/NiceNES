#pragma once

#include <cstdint>

class Mirror {
 public:
    Mirror(uint16_t start, uint16_t end, uint16_t original_start, uint16_t original_end)
        : start(start), end(end),
          original_start(original_start), original_end(original_end) {};
    bool contains(uint16_t addr);
    uint16_t map(uint16_t addr);

 protected:
    uint16_t start, end, original_start, original_end;
};
