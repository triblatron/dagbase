//
// Created by Tony Horrobin on 11/10/2026.
//

#pragma once

#include "config/DagBaseExport.h"

namespace dagbase
{
    class Node;

    class DAGBASE_API Script
    {
    public:
        virtual ~Script() = default;

        virtual void step(Node* self) = 0;

        virtual bool isFinished() const = 0;
    };
}