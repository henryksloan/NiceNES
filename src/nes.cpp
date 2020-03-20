#include "nes.h"

NES::NES() {
    CPUAccessor cpu_accessor;

    // PPU memory map
    auto nametable_mirror = std::make_unique<Mirror>(0x3000, 0x3EFF, 0x2000, 0x2EFF);
    auto palette_mirror = std::make_unique<Mirror>(0x3F20, 0x3FFF, 0x3F00, 0x3F1F);
    std::vector<std::unique_ptr<Mirror>> ppu_mirrors;
    ppu_mirrors.push_back(std::move(nametable_mirror));
    ppu_mirrors.push_back(std::move(palette_mirror));
    auto ppu_ram = std::make_unique<MirroredRAM<0x4000>>(std::move(ppu_mirrors));
    ppu_mapped_ram = std::make_shared<MappedMemory<0x4000>>(std::move(ppu_ram), MapperResult::CHR);
    auto oam_ram = std::make_shared<RAM<0x100>>();
    auto oam2_ram = std::make_shared<RAM<0x20>>();
    ppu = std::make_unique<PPU>(ppu_mapped_ram, oam_ram, oam2_ram, cpu_accessor);

    // CPU memory map
    auto ram_mirror = std::make_unique<Mirror>(0x0800, 0x1FFF, 0x0000, 0x07FF);
    auto ppu_reg_map = std::make_unique<MappedRegisters>(0x2000, 0x2007,
            std::bind(&PPU::register_read, ppu.get(), std::placeholders::_1),
            std::bind(&PPU::register_write, ppu.get(), std::placeholders::_1, std::placeholders::_2));
    auto ppu_reg_mirror = std::make_unique<Mirror>(0x2008, 0x3FFF, 0x2000, 0x2007);
    // auto apu_reg_map = std::make_unique<MappedRegisters>(0x4000, 0x4017,
    //         std::bind(&APU::register_read, &apu, std::placeholders::_1),
    //         std::bind(&APU::register_write, &apu, std::placeholders::_1, std::placeholders::_2));
    std::vector<std::unique_ptr<Mirror>> mirrors;
    mirrors.push_back(std::move(ram_mirror));
    mirrors.push_back(std::move(ppu_reg_mirror));
    std::vector<std::unique_ptr<MappedRegisters>> reg_maps;
    reg_maps.push_back(std::move(ppu_reg_map));
    auto main_ram = std::make_unique<BussedRAM<0x10000>>(std::move(mirrors), std::move(reg_maps));
    cpu_mapped_ram = std::make_shared<MappedMemory<0x10000>>(std::move(main_ram), MapperResult::PRG);
    cpu = std::make_unique<CPU6502>(cpu_mapped_ram);

    cpu_accessor.set_nmi(std::bind(&CPU6502::nmi, cpu.get()));
    cpu_accessor.set_irq(std::bind(&CPU6502::irq, cpu.get()));
    cpu_accessor.set_read_byte(std::bind(&BussedRAM<0x10000>::read_byte, main_ram.get(), std::placeholders::_1));
}

void NES::load_cartridge(std::shared_ptr<Cartridge> cart) {
    cpu_mapped_ram->load_cartridge(cart);
    ppu_mapped_ram->load_cartridge(cart);
    cpu->reset();
    // ppu->reset();

    // std::ofstream cpu_ram_file("cpu_ram.bin", std::ifstream::binary);
    // for (int addr = 0; addr < 0x10000; addr++) {
    //     cpu_ram_file << cpu_mapped_ram->read_byte(addr);
    // }

    // std::ofstream ppu_ram_file("ppu_ram.bin", std::ifstream::binary);
    // for (int addr = 0; addr < 0x10000; addr++) {
    //     ppu_ram_file << ppu_mapped_ram->read_byte(addr);
    // }
}

void NES::cycle() {
    cpu->step();
    ppu->cpu_cycle();
}
