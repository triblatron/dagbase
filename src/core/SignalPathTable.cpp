//
// Created by Tony Horrobin on 08/08/2026.
//

#include "config/config.h"

#include "core/SignalPathTable.h"
#include "core/SignalPath.h"

namespace dagbase
{
    Status SignalPathTable::add(SignalPath *signalPath)
    {
        Status status{dagbase::Status::STATUS_UNKNOWN};

        if (signalPath->from().valid() && signalPath->to().valid())
        {
            _signalPaths.emplace(signalPath->id(), signalPath);
            status.status = dagbase::Status::STATUS_OK;
        }
        else
        {
            status.status = Status::STATUS_SYNTAX_ERROR;
        }
        return status;
    }

    void SignalPathTable::remove(SignalPath *signalPath)
    {
    }

    VectorMap<SignalPathID, SignalPath *>::const_iterator SignalPathTable::findBySource(PortID sourceID) const
    {
        return _signalPaths.end();
    }

    VectorMap<SignalPathID, SignalPath *>::const_iterator SignalPathTable::findByDest(PortID destID) const
    {
        return _signalPaths.end();
    }

    Variant SignalPathTable::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "numSignalPaths", std::uint32_t(size()));
        if (retval.has_value())
            return retval;

        return {};
    }
}
