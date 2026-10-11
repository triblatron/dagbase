//
// Created by Tony Horrobin on 11/10/2026.
//

#pragma once

#include "config/DagBaseExport.h"

namespace dagbase
{
    //! The direction of a port.
    class DAGBASE_API PortDirection
    {
    public:
        enum Direction
        {
            //! Initial or invalid port direction.
            DIR_UNKNOWN,
            //! Input port.
            DIR_IN,
            //! Output port.
            DIR_OUT,
            //! Internal port, cannot be connected.
            DIR_INTERNAL
        };

        //! Parse a string representation of a port direction.
        static Direction parseFromString(const char* str);

        //! Convert a port direction to a string.
        static const char* toString(Direction dir);
    };
}