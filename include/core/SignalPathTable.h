//
// Created by Tony Horrobin on 08/08/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"
#include "util/VectorSet.h"
#include "util/SearchableArray.h"

#include <string_view>


namespace dagbase
{
    class SignalPath;

    struct CompareSignalPaths
    {
        bool operator()(const SignalPath* op1, const SignalPath* op2) const;
    };

    class DAGBASE_API SignalPathTable
    {
    public:
        using FindResult = SearchableArray<std::vector<SignalPath*>>;
    public:
        using LookupTable = VectorSet<SignalPath*, CompareSignalPaths>;

        Status add(SignalPath* signalPath);

        void remove(SignalPath* signalPath);

        std::size_t size() const
        {
            return _signalPaths.size();
        }

        void findBySource(PortID sourceID, FindResult* result) const;

        void findByDest(PortID destID, FindResult* result) const;

        Variant find(std::string_view path) const;
    private:
        LookupTable _signalPaths;
    };
}
