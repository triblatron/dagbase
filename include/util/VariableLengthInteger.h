//
// Created by Tony Horrobin on 18/10/2026.
//

#pragma once

#include <type_traits>
#include <vector>

#include "Zigzag.h"

namespace dagbase
{
    template<typename T, typename=void>
    struct VariableLengthInteger
    {

    };

    template<typename T>
    struct VariableLengthInteger<T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>>
    {
        T value;

        void encode(std::pmr::vector<std::uint8_t>* buf, std::uint8_t moreBit=7) const
        {
            T moreBitMask = T(1)<<moreBit;
            auto extracted = value;

            if (extracted < moreBitMask)
            {
                buf->emplace_back(extracted);
            }
            else
            {
                // The first byte could be a subset of another field.
                std::uint8_t remainingMask = (moreBitMask - 1);
                buf->emplace_back(moreBitMask);
                (*buf)[0] |= (extracted & remainingMask);
                extracted >>= moreBit;
                // Assume we will be using the whole byte for further bytes.
                remainingMask = (1<<7) - 1;
                moreBitMask = 1<<7;
                std::size_t byteIndex=1;
                while (extracted>0)
                {
                    buf->emplace_back(extracted & remainingMask);
                    extracted >>= 7;
                    if (extracted>0)
                        (*buf)[byteIndex] |= moreBitMask;
                    byteIndex++;
                }
            }


        }

        void decode(const std::pmr::vector<std::uint8_t>& buf, std::uint8_t moreBit=7)
        {
            std::uint8_t moreBitMask = 1<<moreBit;
            auto decoded = static_cast<T>(buf[0] & ~moreBitMask);
            std::uint8_t shift = moreBit;
            for (std::size_t byteIndex=1; byteIndex<buf.size(); ++byteIndex)
            {
                decoded |= T(buf[byteIndex] & 127u) << T(shift);
                shift += 7;
            }
            value = decoded;
        }
    };

    template<typename T>
    struct VariableLengthInteger<T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>>
    {
        T value;

        void encode(std::pmr::vector<std::uint8_t>* buf, std::uint8_t moreBit=7) const
        {
            VariableLengthInteger<std::make_unsigned_t<T>> unsignedValue;
            unsignedValue.value = zigzagEncode(value);
            unsignedValue.encode(buf, moreBit);
        }

        void decode(const std::pmr::vector<std::uint8_t>& buf, std::uint8_t moreBit=7)
        {
            VariableLengthInteger<std::make_unsigned_t<T>> unsignedValue;
            unsignedValue.decode(buf, moreBit);
            value = zigzagDecode(unsignedValue.value);
        }
    };
}
