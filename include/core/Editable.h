//
// Created by Tony Horrobin on 15/06/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdint>
#include <string>

struct ImGuiContext;

namespace dagbase
{
    class DAGBASE_API Editable
    {
    public:
        virtual ~Editable() = default;

        virtual void edit(ImGuiContext* context) = 0;

        static void editType(const char* label, std::int64_t* value);

        static void editType(const char* label, bool* value);

        static void editType(const char* label, double* value);

        static void editType(const char* label, std::string* value);
    };
}
