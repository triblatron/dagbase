//
// Created by Tony Horrobin on 10/08/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"
#include "util/VectorMultimap.h"

#include <vector>


namespace dagbase
{
    class Port;

    class DAGBASE_API NodesPortsTable
    {
    public:
        using PortArray = std::vector<Port*>;
        using Table = VectorMultimap<NodeID, PortID>;

        struct FindResult
        {
            using value_type = Table::mapped_type;
            NodesPortsTable& table;

            FindResult(NodesPortsTable& table_)
                :
            table(table_)
            {
                // Do nothing.
            }

            std::size_t size() const
            {
                return std::size_t(std::distance(p.first, p.second));
            }

            Table::value_type operator[](std::size_t index) const
            {
                if (index < size())
                {
                    return *(p.first+index);
                }

                return {};
            }

            Variant find(std::string_view path) const;

            std::pair<Table::const_iterator, Table::const_iterator> p;
        };
    public:
        void portsForNode(NodeID id, FindResult* result) const;

        void addPort(NodeID nodeId, PortID portId);

        Table::const_iterator end() const
        {
            return _lut.end();
        }

        Variant find(std::string_view path) const;
    private:
        Table _lut;
    };
}
