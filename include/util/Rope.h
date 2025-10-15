//
// Created by Tony Horrobin on 12/10/2025.
//

#ifndef DAGUI_ROPE_H
#define DAGUI_ROPE_H

#include "config/DagBaseExport.h"

namespace dagbase
{
    class DAGBASE_API Rope
    {
    public:
        Rope(Rope* left, Rope* right);

        Rope* left()
        {
            return _left;
        }

        Rope* right()
        {
            return _right;
        }
    private:
        Rope* _left{nullptr};
        Rope* _right{nullptr};
    };
}


#endif //DAGUI_ROPE_H
