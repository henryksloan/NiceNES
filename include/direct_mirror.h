#pragma once

#include "mirror.h"

class DirectMirror : public Mirror {
 public:
    DirectMirror(uint16_t start, uint16_t end, uint16_t original_start)
        : Mirror(start, end, original_start) {};
    virtual uint16_t map(uint16_t addr);
};
