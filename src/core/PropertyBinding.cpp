//
// Created by Tony Horrobin on 03/05/2025.
//

#include "config/config.h"

#include "core/PropertyBinding.h"

namespace dagbase
{

    Variant PropertyBinding::interpolate(PropertyBinding::ResolveFunc resolve)
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
                    if (openIndex<_open.length() && c!=_open[openIndex])
                    {
                        //output += c;
                    }
                    else if (openIndex<_open.length() && c==_open[openIndex])
                    {
                        ++openIndex;
                    }
                    if (openIndex==_open.length())
                    {
                        state = STATE_FOUND_OPEN;
                    }
                    break;
                case STATE_FOUND_OPEN:
                    if (closeIndex<_close.length() && c!=_close[closeIndex])
                    {
                        symbol += c;
                    }
                    else if (closeIndex<_close.length() && c==_close[closeIndex])
                    {
                        ++closeIndex;
                    }
                    if (closeIndex==_close.length())
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