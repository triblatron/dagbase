//
// Created by Tony Horrobin on 14/05/2025.
//

#include "config/config.h"

#include "core/Dimension.h"

#include <cstring>
#include <cstdint>
#include <iterator>
#include <cassert>
#include <codecvt>
#include <filesystem>

namespace dagbase
{
    enum PolyState
    {
        POLY_DIM,
        POLY_EXP,
        POLY_POWER
    };

    void Dimension::convertToPolynomial(std::int32_t poly[7]) const
    {
        char symbols[] = { "L,M,T,I,THETA,N,J"};
        // Index into symbols to find dimensions
        std::size_t dims[]= {0,2,4,6,8,14,16};

        PolyState state = POLY_DIM;
        std::size_t dim = 0;
        std::size_t i=0;
        const char* endOfSymbols = symbols + std::strlen(symbols);
        const char* startOfPower=nullptr;
        char* endOfPower=nullptr;
        std::int32_t power = 0;
        std::size_t lenOfSymbol = std::strlen(symbol);
        while (i<lenOfSymbol)
        {
            char c = symbol[i];
            const char* match = nullptr;
            assert(sizeof(dims)/sizeof(std::size_t)==NUM_BASE_DIMENSIONS);
            if (state == POLY_DIM)
            {
                if (c != '^')
                {
                    for (std::size_t j=0; j<NUM_BASE_DIMENSIONS && state == POLY_DIM; ++j)
                    {
                        const char* startOfDim = &symbols[dims[j]];
                        const char* endOfDim = startOfDim;
                        // Find the end of the current dimension in symbols
                        while (endOfDim != endOfSymbols && *endOfDim != ',')
                            ++endOfDim;

                        if (strncmp(startOfDim, &symbol[i], endOfDim-startOfDim)==0)
                        {
                            dim = j;
                            i += endOfDim - startOfDim;

                            state = POLY_EXP;
                            if (i<lenOfSymbol)
                            {
                                c = symbol[i];
                            }
                        }
                    }
                }
                else
                {
                    state = POLY_EXP;
                    ++i;
                    continue;
                }
            }

            if (state == POLY_EXP)
            {
                // Start of a power
                if (std::isdigit(c) || c == '-' || c == '+')
                {
                    startOfPower=&symbol[i];
                    state = POLY_POWER;
                    ++i;
                    continue;
                }
                // No exponent:assume one
                // Note:We do not move to the next character so that it can be consumed by POLY_DIM
                if (std::isalpha(c))
                {
                    poly[dim] = 1;
                    state = POLY_DIM;
                    continue;
                }
                ++i;
            }

            if (state == POLY_POWER)
            {
                // End of power
                if (!std::isdigit(c) && c!='-' & c!='+')
                {
                    power = static_cast<std::int32_t>(std::strtol(startOfPower, &endOfPower, 10 ));

                    poly[dim] = power;
                    state = POLY_DIM;
                    continue;
                }

                ++i;
            }
        }

        if (state == POLY_DIM)
        {
            poly[dim] = 1;
        }
        if (state == POLY_POWER)
        {
            power = static_cast<std::int32_t>(std::strtol(startOfPower, &endOfPower, 10 ));
            if (endOfPower == startOfPower)
            {
                power = 0;
            }
            poly[dim] = power;
        }
    }

    bool Dimension::operator==(const Dimension &other) const
    {
        if (std::strcmp(symbol, other.symbol)==0)
        {
            return true;
        }

        // Convert to polynomial form
        std::int32_t poly1[NUM_BASE_DIMENSIONS]{}, poly2[NUM_BASE_DIMENSIONS]{};
        convertToPolynomial(poly1);
        other.convertToPolynomial(poly2);
        for (std::uint32_t i=0; i<NUM_BASE_DIMENSIONS; ++i)
        {
            if (poly1[i] != poly2[i])
                return false;
        }
        return true;
    }
}
