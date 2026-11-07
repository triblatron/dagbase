#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"
#include "core/Status.h"

#include <vector>

namespace dagbase
{
    class DAGBASE_API Action
    {
    public:
        Action() = default;

        Action(const Action&) = default;

        Action(Action&&) = default;

        Action& operator=(const Action&) = default;

        Action& operator=(Action&&) = default;

        virtual ~Action() = default;

        virtual Status makeItSo(const std::pmr::vector<dagbase::Variant>& args, std::pmr::vector<dagbase::Variant>* results) = 0;
    };
}
