//
// Created by Tony Horrobin on 16/12/2025.
//

#ifndef DAGUI_DUMMYMUTEX_H
#define DAGUI_DUMMYMUTEX_H

#include "config/DagBaseExport.h"

namespace dagbase
{
    class DAGBASE_API DummyMutex
    {
    public:
        void lock()
        {
            // Do nothing.
        }

        void unlock()
        {
            // Do nothing.
        }
    };
}
#endif //DAGUI_DUMMYMUTEX_H