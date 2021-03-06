/*
 * Generic Interrupt Controller: Physical CPU Interface (GICC/ICC)
 *
 * Copyright (C) 2019-2020 Udo Steinberg, BedRock Systems, Inc.
 *
 * This file is part of the NOVA microhypervisor.
 *
 * NOVA is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NOVA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 */

#include "assert.hpp"
#include "gicc.hpp"
#include "hpt.hpp"
#include "pd_kern.hpp"
#include "stdio.hpp"

Gicc::Mode Gicc::mode;

void Gicc::init()
{
    // Reserve MMIO region even if we use REGS
    if (Cpu::bsp)
        Pd_kern::remove_user_mem (Board::gic[2].mmio, Board::gic[2].size);

    init_mode();

    switch (mode) {
        case Mode::REGS: init_regs(); break;
        case Mode::MMIO: init_mmio(); break;
    }
}

void Gicc::init_regs()
{
    trace (TRACE_INTR, "GICC: REGS");

    set_el1_pmr     (0xff);
    set_el1_bpr1    (0x7);
    set_el1_igrpen1 (BIT (0));
    set_el1_ctlr    (BIT (1));

    // Ensure system register writes executed
    Barrier::isb();
}

void Gicc::init_mmio()
{
    if (!Board::gic[2].size)
        return;

    if (Cpu::bsp) {

        unsigned offs = Board::gic[2].size == 0x20000 ? 0xf000 : 0;

        for (unsigned i = 0; i < 2; i++)
            Hptp::master.update (DEV_GLOBL_GICC + i * PAGE_SIZE, Board::gic[2].mmio + offs + i * PAGE_SIZE, 0,
                                 Paging::Permissions (Paging::G | Paging::W | Paging::R),
                                 Memattr::Cacheability::DEV, Memattr::Shareability::NONE);
    }

    auto iidr = read (Register32::IIDR);

    trace (TRACE_INTR, "GICC: %#010llx Impl:%#x Prod:%#x r%up%u",
           Board::gic[2].mmio,
           iidr & 0xfff, iidr >> 20, iidr >> 16 & 0xf, iidr >> 12 & 0xf);

    // Disable interrupt signaling
    write (Register32::CTLR, 0);

    write (Register32::PMR, 0xff);
    write (Register32::BPR, 0x7);

    // Enable interrupt signaling
    write (Register32::CTLR, BIT (9) | BIT (6) | BIT (5) | BIT (0));
}

void Gicc::init_mode()
{
    mode = Mode::MMIO;

    if (Cpu::feature (Cpu::Cpu_feature::GIC)) {

        // Disable IRQ/FIQ bypass and enable system registers
        set_el2_sre (get_el2_sre() | BIT_RANGE (2, 0));

        // Ensure system register write executed
        Barrier::isb();

        if (get_el2_sre() & BIT (0))
            mode = Mode::REGS;
    }
}

void Gicc::send_sgi (unsigned cpu, unsigned sgi)
{
    assert (sgi < NUM_SGI && mode == Mode::REGS);

    uint64 affinity = Cpu::remote_affinity (cpu);

    // Ensure completion (global observability) of all previous stores
    Barrier::wmb_sync();

    set_el1_sgi1r (sgi << 24 | (affinity & BIT_RANGE (31, 24)) << 24 |  // Aff3
                               (affinity & BIT_RANGE (23, 16)) << 16 |  // Aff2
                               (affinity & BIT_RANGE (15,  8)) <<  8 |  // Aff1
                           BIT (affinity & BIT_RANGE ( 3,  0)));        // Aff0

    // Ensure system register write executed
    Barrier::isb();
}
