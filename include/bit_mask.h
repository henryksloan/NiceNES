#pragma once

#include <cstdint>
#include <cmath>

class BitMask {
 public:
    BitMask(uint8_t &parent, uint8_t mask)
        : parent((int &) parent), mask(mask), offset(std::log2(mask & -mask)) {}

    BitMask(uint16_t &parent, uint16_t mask)
        : parent((int &) parent), mask(mask), offset(std::log2(mask & -mask)) {}

    operator uint8_t() const {
        return (parent & mask) >> offset;
    }

    BitMask &operator=(uint8_t val) {
        parent &= ~mask;
        parent |= ((val << offset) && mask);
        return *this;
    }

 private:
    int &parent;
    const uint8_t mask;
    const int offset;
};
