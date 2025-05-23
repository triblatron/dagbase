//
// Created by Tony Horrobin on 03/05/2025.
//

#include "config/config.h"

#include "core/PropertyBinding.h"

namespace dagbase
{

    Variant PropertyBinding::interpolate(Atom open, Atom close, PropertyBinding::ResolveFunc resolve)
    {
        Variant output;
        State state = STATE_INITIAL;
        std::size_t openIndex = 0;
        std::size_t closeIndex = 0;
        std::string symbol;

        for (auto c : _input)
        {
            switch (state)
            {
                case STATE_INITIAL:
                    if (openIndex<open.length() && c!=open[openIndex])
                    {
                        //output += c;
                    }
                    else if (openIndex<open.length() && c==open[openIndex])
                    {
                        ++openIndex;
                    }
                    if (openIndex==open.length())
                    {
                        state = STATE_FOUND_OPEN;
                    }
                    break;
                case STATE_FOUND_OPEN:
                    if (closeIndex<close.length() && c!=close[closeIndex])
                    {
                        symbol += c;
                    }
                    else if (closeIndex<close.length() && c==close[closeIndex])
                    {
                        ++closeIndex;
                    }
                    if (closeIndex==close.length())
                    {
                        output = resolve(symbol);
                        openIndex = 0;
                        closeIndex = 0;
                        symbol.clear();
                        state = STATE_INITIAL;
                    }
                    break;
                case STATE_FOUND_CLOSE:

                    state = STATE_INITIAL;
                    break;
            }
        }

        return output;
    }
}