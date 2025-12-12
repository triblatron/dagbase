//
// Created by Tony Horrobin on 11/12/2025.
//

#include "config/config.h"

#include "util/Base64.h"

namespace dagbase
{
    std::uint8_t toBase64(std::uint8_t inputByte)
    {
        if (inputByte >=0 && inputByte <= 25)
            return inputByte + 'A';
        if (inputByte >= 26 && inputByte <= 51)
            return inputByte + 'a' - 26;
        if (inputByte >= 52 && inputByte <= 61)
            return inputByte + '0' - 52;
        if (inputByte == 62)
            return '+';
        if (inputByte == 63)
            return '/';

        return 0;
    }

    std::uint8_t fromBase64(std::uint8_t inputByte)
    {
        if (inputByte >= 'A' && inputByte <= 'Z')
        {
            return inputByte - 'A';
        }
        if (inputByte >= 'a' && inputByte <= 'z')
        {
            return inputByte - 'a' + 26;
        }
        if (inputByte >= '0' && inputByte <= '9')
        {
            return inputByte - '0' + 52;
        }
        if (inputByte == '+')
        {
            return 62;
        }
        if (inputByte == '/')
        {
            return 63;
        }

        return 0;
    }

    void base64encode(const std::uint8_t *input, std::size_t inputLen, std::vector<std::uint8_t> *output)
    {
        if (!output)
            return;

        // Take chunks of 6 bits, mapping to a base64 character
        std::size_t inputIndex=0;
        output->reserve(static_cast<std::size_t>(4.0/3.0 * static_cast<double>(inputLen)));
        std::size_t numGroups = inputLen / 3;
        std::size_t numPadding = 3 - (inputLen % 3);
        // Deal with whole groups of 3 6-bit numbers
        for (std::size_t groupIndex=0; groupIndex<numGroups; ++groupIndex)
        {
            // First six bits of first byte shifted into low six bits
            std::uint8_t inputByte = input[inputIndex] >> 2;
            std::uint8_t outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            // Lower two bits of first byte shifted into high bits of second input byte 
            inputByte = (input[inputIndex++] & ((1<<2)-1))<<4;
            // High four bits of second input byte into loewr four bits of second output six bits
            inputByte+= input[inputIndex] >> 4;
            outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            // Low four bits of second input byte into high four bits of third output byte
            inputByte = (input[inputIndex++] & ((1<<4)-1))<<2;
            // High two bits of third input byte into low two bits of third output byte
            inputByte+= input[inputIndex] >> 6;
            outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            // Low six bits of third input byte into fourth output byte
            
            inputByte = input[inputIndex++] & ((1<<6)-1);
            outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);

        }
        // Deal with padding
        if (numPadding==1)
        {
            std::uint8_t inputByte = input[inputIndex] >> 2;
            std::uint8_t outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            inputByte = (input[inputIndex++] & ((1<<2)-1))<<4;
            inputByte+= input[inputIndex] >> 4;
            outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            inputByte = (input[inputIndex++] & ((1<<4)-1))<<2;
            inputByte+= input[inputIndex] >> 6;
            // Discard LS two bits of last block
            inputByte &= ~0x3;
            outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            output->emplace_back('=');
        }
        else if (numPadding==2)
        {
            std::uint8_t inputByte = input[inputIndex] >> 2;
            std::uint8_t outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            inputByte = (input[inputIndex] & ((1<<2)-1))<<4;
            // Discard 4 LSBs of last block
//            inputByte+= input[inputIndex] >> 4;
            outputByte = toBase64(inputByte);
            output->emplace_back(outputByte);
            output->emplace_back('=');
            output->emplace_back('=');
        }
    }

    void base64decode(const std::uint8_t *input, std::size_t inputLen, std::vector<std::uint8_t> *output)
    {
        if (!output)
            return;

        std::size_t inputIndex=0;
        output->reserve((inputLen>>2)*3);
        std::size_t numGroups = inputLen>>2;
        std::size_t numPadding = 3 - (inputLen & 3);
        for (std::size_t groupIndex=0; groupIndex<numGroups; ++groupIndex)
        {
            std::uint8_t decoded[4] = {
            fromBase64(input[inputIndex]),
            fromBase64(input[inputIndex+1]),
            fromBase64(input[inputIndex+2]),
            fromBase64(input[inputIndex+3])
            };
            std::uint8_t outputByte = (decoded[0] << 2) + (decoded[1] >>4);

            output->emplace_back(outputByte);
            outputByte = ((decoded[1] & 0xf)<<4) + (decoded[2]>>2);
            if (input[inputIndex + 2] != '=')
            {
                output->emplace_back(outputByte);
            }

            outputByte = ((decoded[2] & 0x03)<<6) + (decoded[3]);
            if (input[inputIndex + 3] != '=')
            {
                output->emplace_back(outputByte);
            }

            inputIndex+=4;
        }
    }
}
