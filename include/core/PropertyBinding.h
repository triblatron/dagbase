//
// Created by Tony Horrobin on 03/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"
#include "core/Atom.h"

#include <string>
#include <functional>

namespace dagbase
{
    class DAGBASE_API PropertyBinding
    {
    public:
        enum State
        {
            STATE_INITIAL,
            STATE_FOUND_OPEN,
            STATE_FOUND_CLOSE
        };
        using ResolveFunc = std::function<dagbase::Variant(std::string)>;
    public:
        virtual ~PropertyBinding() = default;

        void setInput(const char* input)
        {
            if (input)
                _input = input;
        }

        Variant interpolate(Atom open, Atom close, ResolveFunc resolve);
    private:
        std::string _input;
    };
}