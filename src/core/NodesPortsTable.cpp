//
// Created by Tony Horrobin on 10/08/2026.
//

#include "config/config.h"

#include "core/NodesPortsTable.h"

namespace dagbase
{
    Variant NodesPortsTable::FindResult::find(std::string_view path) const
    {
        return {};
    }

    void NodesPortsTable::portsForNode(NodeID id, FindResult *result) const
    {
    }

    void NodesPortsTable::addPort(NodeID nodeId, PortID portId)
    {
        _lut.emplace(nodeId, portId);
    }

    Variant NodesPortsTable::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "size", std::uint32_t(_lut.size()));
        if (retval.has_value())
            return retval;

        return {};
    }
}
