//
// Created by Tony Horrobin on 25/04/2025.
//

#include "core/Snippet.h"
#include "core/ConfigurationElement.h"

namespace dagbase
{

    std::string Snippet::interpolate()
    {
        std::string output;
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
                        output += c;
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
                        output += resolve(symbol);
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

    void Snippet::configure(ConfigurationElement &config)
    {
        if (auto element=config.findElement("input"); element)
        {
            _input = element->asString();
        }

        if (auto element = config.findElement("open"); element)
        {
            _open = Atom::intern(element->asString());
        }

        if (auto element = config.findElement("close"); element)
        {
            _close = Atom::intern(element->asString());
        }
    }
}