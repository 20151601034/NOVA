/*
 * Page Table Entry (PTE)
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

#pragma once

#include "hpt.hpp"
#include "npt.hpp"

template<> unsigned Pte<Hpt, 4, 9, uint64, uint64>::lim = 3 * 9 - 1;
template<> unsigned Pte<Npt, 3, 9, uint64, uint64>::lim = 3 * 9 - 1;

template class Pte<Hpt, 4, 9, uint64, uint64>;
template class Pte<Npt, 3, 9, uint64, uint64>;
