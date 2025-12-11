//
// Created by Tony Horrobin on 11/12/2025.
//

#ifndef DAGUI_BASE64_H
#define DAGUI_BASE64_H

#include "config/DagBaseExport.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>

namespace dagbase
{
    void DAGBASE_API base64encode(const std::uint8_t* input, std::size_t inputLen, std::vector<std::uint8_t>* output );
    void DAGBASE_API base64decode(const std::uint8_t* input, std::size_t inputLen, std::vector<std::uint8_t>* output );
}
#endif //DAGUI_BASE64_H
