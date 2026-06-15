//
// Created by Tony Horrobin on 15/06/2026.
//

#pragma once

#include "config/DagBaseExport.h"

struct ImGuiContext;

namespace dagbase
{
    class DAGBASE_API Editable
    {
    public:
        virtual ~Editable() = default;

        virtual void edit(ImGuiContext* context) = 0;
    };
}