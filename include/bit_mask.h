#pragma once

#include <cstdint>
#include <cmath>

class BitMask {
 public:
    BitMask(uint8_t &parent, uint8_t mask)
        : parent((int &) parent), mask(mask), offset(std::log2(mask & -mask)) {}

    BitMask(uint16_t &parent, uint16_t mask)
        : parent((int &) parent), mask(mask), offset(std::log2(mask & -mask)) {}

    operator int() const {
        return (parent & mask) >> offset;
    }

    BitMask &operator=(int val) {
        parent &= ~mask;
        parent |= ((val << offset) && mask);
        return *this;
    }

    BitMask &operator=(const BitMask &other) {
        return operator=((int) other);
    }

 private:
    int &parent;
    const int mask;
    const int offset;
};
