#pragma once

#include "mirror.h"

class RepeatMirror : public Mirror {
 public:
    RepeatMirror(uint16_t start, uint16_t end, uint16_t original_start, uint16_t original_end)
        : Mirror(start, end, original_start), len(original_end-original_start+1) {};
    virtual uint16_t map(uint16_t addr);

 private:
    uint16_t len;
};
