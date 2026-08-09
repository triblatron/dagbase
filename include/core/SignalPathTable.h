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

    struct CompareSignalPathsByFrom
    {
        bool operator()(const SignalPath* op1, const SignalPath* op2) const;
    };

    struct CompareSignalPathsByTo
    {
        bool operator()(const SignalPath* op1, const SignalPath* op2) const;
    };

    class DAGBASE_API SignalPathTable
    {
    public:
        using LookupTableFrom = VectorSet<SignalPath*, CompareSignalPathsByFrom>;
        using LookupTableTo = VectorSet<SignalPath*, CompareSignalPathsByTo>;

        struct DAGBASE_API FindResult
        {
            using value_type = SignalPath *;
            Variant find(std::string_view path) const;

            std::size_t size() const
            {
                return std::size_t(std::distance(p.first,p.second));
            }

            const SignalPath* operator[](std::size_t index) const
            {
                if (index<std::size_t(std::distance(p.first,p.second)))
                    return *(p.first+LookupTableFrom::difference_type(index));

                return nullptr;
            }

            std::pair<LookupTableFrom::const_iterator, LookupTableFrom::const_iterator> p;
        };
    public:
        Status add(SignalPath* signalPath);

        void remove(SignalPath* signalPath);

        std::size_t size() const
        {
            return _signalPathsFrom.size();
        }

        void findBySource(PortID sourceID, FindResult* result) const;

        void findByDest(PortID destID, FindResult* result) const;

        void findFull(PortID sourceID, PortID destID, FindResult* result) const;

        Variant find(std::string_view path) const;
    private:
        LookupTableFrom _signalPathsFrom;
        LookupTableTo _signalPathsTo;
    };
}
