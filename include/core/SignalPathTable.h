//
// Created by Tony Horrobin on 08/08/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"
#include "util/VectorSet.h"
#include "util/VectorMap.h"
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
        using LookupTableId = VectorMap<SignalPathID, SignalPath*>;
        using LookupTableFrom = VectorSet<SignalPath*, CompareSignalPathsByFrom>;
        using LookupTableTo = VectorSet<SignalPath*, CompareSignalPathsByTo>;

        struct DAGBASE_API FindResultFrom
        {
            using value_type = SignalPath *;
            Variant find(std::string_view path) const;

            std::size_t size() const
            {
                return static_cast<std::size_t>(std::distance(p.first, p.second));
            }

            const SignalPath* operator[](std::size_t index) const
            {
                if (index<static_cast<std::size_t>(std::distance(p.first,p.second)))
                    return *(p.first+static_cast<LookupTableFrom::difference_type>(index));

                return nullptr;
            }

            std::pair<LookupTableFrom::const_iterator, LookupTableFrom::const_iterator> p;
        };
    public:
        Status add(SignalPath* signalPath);

        Status remove(SignalPathID id);

        std::size_t size() const
        {
            return _signalPathsFrom.size();
        }

        SignalPath* findByID(SignalPathID id) const;

        void findBySource(PortID sourceID, FindResultFrom* result) const;

        void findByDest(PortID destID, FindResultFrom* result) const;

        void findFull(PortID sourceID, PortID destID, FindResultFrom* result) const;

        Variant find(std::string_view path) const;
    private:
        LookupTableId _signalPathsByID;
        LookupTableFrom _signalPathsFrom;
        LookupTableTo _signalPathsTo;
    };
}
