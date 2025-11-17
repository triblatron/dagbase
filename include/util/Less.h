//
// Created by Tony Horrobin on 17/11/2025.
//

#ifndef DAGUI_LESS_H
#define DAGUI_LESS_H

#include <utility>

namespace dagbase
{
    template<typename T1, typename T2>
    struct Less
    {
        bool operator()(const std::pair<T1, T2>& op1, const std::pair<T1, T2>& op2) const
        {
            return op1.first < op2.first;
        }
    };
}

#endif //DAGUI_LESS_H
