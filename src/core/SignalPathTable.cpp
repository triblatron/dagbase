//
// Created by Tony Horrobin on 08/08/2026.
//

#include "config/config.h"

#include "core/SignalPathTable.h"
#include "core/SignalPath.h"

#include <iostream>

namespace dagbase
{
    bool CompareSignalPathsByFrom::operator()(const SignalPath *op1, const SignalPath * op2) const
    {
        if (op1->from() < op2->from())
            return true;

        if (op1->from() == op2->from())
        {
            if (op2->to() != PortID::INVALID_ID && op1->to() < op2->to())
                return true;

            if (op2->to() != PortID::INVALID_ID && op1->to() == op2->to())
            {
                if (op2->id() != SignalPathID::INVALID_ID && op1->id() < op2->id())
                    return true;
            }
        }

        return false;
    }

    bool CompareSignalPathsByTo::operator()(const SignalPath *op1, const SignalPath * op2) const
    {
        if (op1->to() < op2->to())
            return true;

        if (op1->to() == op2->to())
        {
            if (op2->from() != PortID::INVALID_ID && op1->from() < op2->from())
                return true;

            if (op2->from() != PortID::INVALID_ID && op1->from() == op2->from())
            {
                if (op2->id() != SignalPathID::INVALID_ID && op1->id() < op2->id())
                    return true;
            }
        }

        return false;
    }

    Variant SignalPathTable::FindResultFrom::find(std::string_view path) const
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

        if (signalPath && signalPath->from().valid() && signalPath->to().valid())
        {
            _signalPathsByID.emplace(signalPath->id(), signalPath);
            _signalPathsFrom.emplace(signalPath);
            _signalPathsTo.emplace(signalPath);
            status.resultType = Status::RESULT_SIGNAL_PATH_ID;
            status.result.emplace(signalPath->id());
            status.status = dagbase::Status::STATUS_OK;
        }
        else
        {
            status.status = Status::STATUS_SYNTAX_ERROR;
        }
        return status;
    }

    Status SignalPathTable::remove(SignalPathID id)
    {
        Status status{Status::STATUS_UNKNOWN};

        if (auto it=_signalPathsByID.find(id); it!=_signalPathsByID.end())
        {
            // debug(std::cerr);
            if (auto itFrom = _signalPathsFrom.find(it->second); itFrom != _signalPathsFrom.end())
                _signalPathsFrom.erase(itFrom);
            if (auto itTo = _signalPathsTo.find(it->second); itTo != _signalPathsTo.end())
                _signalPathsTo.erase(itTo);
            _signalPathsByID.erase(it);
            assert(_signalPathsByID.size() == _signalPathsFrom.size());
            assert(_signalPathsTo.size() == _signalPathsFrom.size());
            status.status = Status::STATUS_OK;
        }

        return status;
    }

    void SignalPathTable::erase(LookupTableId::iterator it)
    {
        if (it!=_signalPathsByID.end())
        {
            if (auto itFrom = _signalPathsFrom.find(it->second); itFrom != _signalPathsFrom.end())
                _signalPathsFrom.erase(itFrom);
            if (auto itTo = _signalPathsTo.find(it->second); itTo != _signalPathsTo.end())
                _signalPathsTo.erase(itTo);
            _signalPathsByID.erase(it);
        }
    }

    void SignalPathTable::erase(const std::vector<SignalPath*>& toRemove, LookupTableId::iterator first, LookupTableId::iterator last)
    {
        auto keepFrom = std::remove_if(_signalPathsFrom.begin(), _signalPathsFrom.end(), [this, first](const SignalPath* signalPath) {
            return _signalPathsByID.find(signalPath->id()) == _signalPathsByID.end();
            }
        );
        _signalPathsFrom.erase(keepFrom, _signalPathsFrom.end());
        auto keepTo = std::remove_if(_signalPathsTo.begin(), _signalPathsTo.end(), [this, first](const SignalPath* signalPath) {
            return _signalPathsByID.find(signalPath->id()) == _signalPathsByID.end();
            }
        );
        _signalPathsTo.erase(keepTo, _signalPathsTo.end());
        _signalPathsByID.erase(first,last);
        assert(_signalPathsByID.size() == _signalPathsFrom.size());
        assert(_signalPathsFrom.size() == _signalPathsTo.size());
    }

    SignalPath * SignalPathTable::findByID(SignalPathID id) const
    {
        if (auto it=_signalPathsByID.find(id); it!=_signalPathsByID.end() && !it->second->isRemoved())
        {
            return it->second;
        }

        return nullptr;
    }

    void SignalPathTable::findBySource(PortID sourceID, FindResultFrom *result) const
    {
        if (result)
        {
            SignalPath temp(_parent, sourceID, PortID());

            result->p.first = _signalPathsFrom.findPartial(&temp);

            for (result->p.second=result->p.first; result->p.second != _signalPathsFrom.end() && (*result->p.second)->from() == sourceID; ++result->p.second);
        }
    }

    void SignalPathTable::findByDest(PortID destID, FindResultFrom *result) const
    {
        if (result)
        {
            SignalPath temp( _parent, PortID(), destID);

            result->p.first = _signalPathsTo.findPartial(&temp);

            for (result->p.second=result->p.first; result->p.second != _signalPathsTo.end() && (*result->p.second)->to() == destID; ++result->p.second);
        }
    }

    void SignalPathTable::findFull(PortID sourceID, PortID destID, FindResultFrom *result) const
    {
        if (result)
        {
            SignalPath temp(_parent, sourceID, destID);

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

        retval = findMapForward(path, _signalPathsByID);
        if (retval.has_value())
            return retval;

        return {};
    }

    void SignalPathTable::debug(std::ostream &str) const
    {
        str << "SignalPathTable:byID:\n";
        for (auto p : _signalPathsByID)
        {
            str << *p.second << '\n';
        }
        str << "SignalPathTable::byFrom:\n";
        for (auto p : _signalPathsFrom)
        {
            str << *p << '\n';
        }
        str << "SignalPathTable::byTo:\n";
        for (auto p : _signalPathsTo)
        {
            str << *p << '\n';
        }
    }
}
