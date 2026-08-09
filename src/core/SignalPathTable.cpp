//
// Created by Tony Horrobin on 08/08/2026.
//

#include "config/config.h"

#include "core/SignalPathTable.h"
#include "core/SignalPath.h"

namespace dagbase
{
    bool CompareSignalPaths::operator()(const SignalPath *op1, const SignalPath * op2) const
    {
        if (op1->from() < op2->from())
            return true;

        if (op1->from() == op2->from() && op1->to() < op2->to())
            return true;

        return false;
    }

    Status SignalPathTable::add(SignalPath *signalPath)
    {
        Status status{dagbase::Status::STATUS_UNKNOWN};

        if (signalPath->from().valid() && signalPath->to().valid())
        {
            _signalPaths.emplace(signalPath);
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

    void SignalPathTable::findBySource(PortID sourceID, FindResult *result) const
    {
        if (result)
        {
            SignalPath temp(sourceID, PortID::INVALID_ID);

            auto it = _signalPaths.findPartial(&temp);
            while (it != _signalPaths.end() && (*it)->from() == sourceID)
            {
                result->a.emplace_back(*it);
                ++it;
            }
        }
    }

    void SignalPathTable::findByDest(PortID destID, FindResult *result) const
    {
        if (result)
        {
            SignalPath temp( PortID::INVALID_ID, destID);

            auto it = _signalPaths.findPartial(&temp);
            while (it != _signalPaths.end() && (*it)->to() == destID)
            {
                result->a.emplace_back(*it);
                ++it;
            }
        }
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
