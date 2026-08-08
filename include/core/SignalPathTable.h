//
// Created by Tony Horrobin on 08/08/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"

#include "util/VectorMap.h"

#include <string_view>

namespace dagbase
{
    class SignalPath;

    class DAGBASE_API SignalPathTable
    {
    public:
        using LookupTable = VectorMap<SignalPathID, SignalPath*>;

        Status add(SignalPath* signalPath);

        void remove(SignalPath* signalPath);

        std::size_t size() const
        {
            return _signalPaths.size();
        }

        LookupTable::const_iterator findBySource(PortID sourceID) const;

        LookupTable::const_iterator findByDest(PortID destID) const;

        Variant find(std::string_view path) const;
    private:
        LookupTable _signalPaths;
    };
}
