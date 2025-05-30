//
// Created by Tony Horrobin on 25/04/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "core/Atom.h"

#include <string>

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API Snippet
    {
    public:
        enum State
        {
            STATE_INITIAL,
            STATE_FOUND_OPEN,
            STATE_FOUND_CLOSE
        };
    public:
        virtual ~Snippet() = default;

        void setInput(const char* input)
        {
            if (input)
            {
                _input = input;
            }
        }

        void setOpen(Atom open)
        {
            _open = open;
        }

        void setClose(Atom close)
        {
            _close = close;
        }

        virtual void configure(ConfigurationElement& config);

        virtual std::string resolve(const std::string& name) = 0;

        std::string interpolate();
    private:
        std::string _input;
        Atom _open;
        Atom _close;
    };
}