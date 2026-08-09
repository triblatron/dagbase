//
// Created by Tony Horrobin on 08/08/2026.
//

#include "config/config.h"

#include "core/SignalPathTable.h"
#include "core/SignalPath.h"

namespace dagbase
{
    bool CompareSignalPathsByFrom::operator()(const SignalPath *op1, const SignalPath * op2) const
    {
        if (op1->from() < op2->from())
            return true;

        if (op1->from() == op2->from() && op1->to() < op2->to())
            return true;

        return false;
    }

    bool CompareSignalPathsByTo::operator()(const SignalPath *op1, const SignalPath * op2) const
    {
        if (op1->to() < op2->to())
            return true;

        if (op1->to() == op2->to() && op1->from() < op2->from())
            return true;

        return false;
    }

    Variant SignalPathTable::FindResult::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "size", std::uint32_t(std::distance(p.first,p.second)));
        if (retval.has_value())
            return retval;

        retval = findArray(path, *this);
        if (retval.has_value())
            return retval;

        return {};
    }

    Status SignalPathTable::add(SignalPath *signalPath)
    {
        Status status{dagbase::Status::STATUS_UNKNOWN};

        if (signalPath->from().valid() && signalPath->to().valid())
        {
            _signalPathsFrom.emplace(signalPath);
            _signalPathsTo.emplace(signalPath);
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

            result->p.first = _signalPathsFrom.findPartial(&temp);

            for (result->p.second=result->p.first; result->p.second != _signalPathsFrom.end() && (*result->p.second)->from() == sourceID; ++result->p.second);
        }
    }

    void SignalPathTable::findByDest(PortID destID, FindResult *result) const
    {
        if (result)
        {
            SignalPath temp( PortID::INVALID_ID, destID);

            result->p.first = _signalPathsTo.findPartial(&temp);

            for (result->p.second=result->p.first; result->p.second != _signalPathsTo.end() && (*result->p.second)->to() == destID; ++result->p.second);
        }
    }

    void SignalPathTable::findFull(PortID sourceID, PortID destID, FindResult *result) const
    {
        if (result)
        {
            SignalPath temp(sourceID, destID);

            result->p.first = _signalPathsFrom.findPartial(&temp);

            for (result->p.second=result->p.first; result->p.second != _signalPathsFrom.end() && (*result->p.second)->from() == sourceID && (*result->p.second)->to() == destID; ++result->p.second);
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
