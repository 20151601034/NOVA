/*
 * Command Line Parser
 *
 * Copyright (C) 2009-2011 Udo Steinberg <udo@hypervisor.org>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * Copyright (C) 2012-2013 Udo Steinberg, Intel Corporation.
 * Copyright (C) 2014 Udo Steinberg, FireEye, Inc.
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

#include "types.hpp"

class Cmdline
{
    public:
        static bool iommu;
        static bool serial;
        static bool nodl;
        static bool nopcid;
        static bool noquiesce;
        static bool novpid;

        static void parse (char const *);

    private:
        static constexpr struct
        {
            char const *    str;
            bool &          var;
        } options[] =
        {
            { "iommu",      iommu       },
            { "serial",     serial      },
            { "nodl",       nodl        },
            { "nopcid",     nopcid      },
            { "noquiesce",  noquiesce   },
            { "novpid",     novpid      },
        };

        static inline size_t arg_len (char const *&);
};
