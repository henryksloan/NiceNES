#pragma once

#include <functional>

class CPUAccessor {
 public:
    void set_read_byte(std::function<uint8_t(uint16_t)> read_byte_f) { this->read_byte_f = read_byte_f; };
    void set_nmi(std::function<void()> nmi_f) { this->nmi_f = nmi_f; };
    void set_irq(std::function<void()> irq_f) { this->irq_f = irq_f; };

    uint8_t read_byte(uint16_t addr) const { return read_byte_f(addr); };
    void nmi() const { nmi_f(); };
    void irq() const { irq_f(); };

 private:
    std::function<uint8_t(uint16_t)> read_byte_f;
    std::function<void()> nmi_f;
    std::function<void()> irq_f;
};
