#pragma once

#include <cstdint>
#include <functional>

class MappedRegisters {
 public:
    MappedRegisters(uint16_t start, uint16_t end,
            std::function<uint8_t(uint16_t)> read,
            std::function<uint8_t&(uint16_t)> ref,
            std::function<void(uint16_t, uint8_t)> write)
        : read(read), ref(ref), write(write), start(start), end(end) {}

    const std::function<uint8_t(uint16_t)> read;
    const std::function<uint8_t&(uint16_t)> ref;
    const std::function<void(uint16_t, uint8_t)> write;

    bool contains(uint16_t addr) { return (addr >= start) && (addr <= end); }

 private:
    uint16_t start, end;
};
