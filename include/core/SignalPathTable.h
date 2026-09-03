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

#include "SignalPath.h"


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
        using LookupTableId = VectorMapFromId<SignalPathID, SignalPath*>;
        using LookupTableFrom = VectorSet<SignalPath*, CompareSignalPathsByFrom>;
        using LookupTableTo = VectorSet<SignalPath*, CompareSignalPathsByTo>;

        struct DAGBASE_API FindResultFrom
        {
            using value_type = SignalPath *;
            Variant find(std::string_view path) const;

            bool empty() const
            {
                return p.first == p.second;
            }

            std::size_t size() const
            {
                return static_cast<std::size_t>(std::distance(p.first, p.second));
            }

            LookupTableFrom::const_iterator begin() const
            {
                return p.first;
            }

            LookupTableFrom::const_iterator end() const
            {
                return p.second;
            }

            const SignalPath* operator[](std::size_t index) const
            {
                if (index<static_cast<std::size_t>(std::distance(p.first,p.second)))
                {
                    if (const auto result = *(p.first+static_cast<LookupTableFrom::difference_type>(index)); !result->isRemoved())
                        return result;
                }

                return nullptr;
            }

            std::pair<LookupTableFrom::const_iterator, LookupTableFrom::const_iterator> p;
        };
    public:
        Status add(SignalPath* signalPath);

        Status remove(SignalPathID id);

        void erase(LookupTableId::iterator it);

        void erase(LookupTableId::iterator first, LookupTableId::iterator last);

        std::size_t size() const
        {
            return _signalPathsFrom.size();
        }

        LookupTableId::const_iterator begin() const
        {
            return _signalPathsByID.begin();
        }

        LookupTableId::iterator begin()
        {
            return _signalPathsByID.begin();
        }

        LookupTableId::const_iterator end() const
        {
            return _signalPathsByID.end();
        }

        LookupTableId::iterator end()
        {
            return _signalPathsByID.end();
        }

        //! Find a SignalPath by ID.
        //! \retval A valid pointer if there exists a SignalPath not marked as removed with the specified ID.
        //! \retval nullptr otherwise
        SignalPath* findByID(SignalPathID id) const;

        //! Find the set of SignalPaths that have the specified source ID.
        //! \note The result may contain SignalPaths that have been marked removed.
        void findBySource(PortID sourceID, FindResultFrom* result) const;

        //! Find the set of SignalPaths that have the specified destination ID.
        //! \note The result may contain SignalPaths that have been marked removed.
        void findByDest(PortID destID, FindResultFrom* result) const;

        //! Find the SignalPath with the specified source and destination ID.
        //! \note The result may contain a SignalPath that have been marked removed.
        void findFull(PortID sourceID, PortID destID, FindResultFrom* result) const;

        Variant find(std::string_view path) const;
    private:
        LookupTableId _signalPathsByID;
        LookupTableFrom _signalPathsFrom;
        LookupTableTo _signalPathsTo;
    };
}
