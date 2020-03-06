#pragma once

#include "bit_mask.h"

#include <cstdint>

template<typename T>
struct RegisterTemplate {
    T raw;

    operator T() const { return raw; }
    T &operator=(T val) {
        raw = val;
        return raw;
    }
};

typedef struct Register : RegisterTemplate<uint8_t> {
    using RegisterTemplate<uint8_t>::operator uint8_t;
    using RegisterTemplate<uint8_t>::operator=;
} Register;

typedef struct Register16 : RegisterTemplate<uint16_t> {
    const BitMask hi{raw, 0xFF00};
    const BitMask lo{raw, 0x00FF};

    using RegisterTemplate<uint16_t>::operator uint16_t;
    using RegisterTemplate<uint16_t>::operator=;
} Register16;
