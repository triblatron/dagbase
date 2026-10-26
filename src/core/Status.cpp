//
// Created by Tony Horrobin on 11/10/2026.
//

#include "config/config.h"

#include "core/Status.h"
#include "util/enums.h"

#include <iostream>

namespace dagbase
{
        //STATUS_OK,
    //    //! A file could not be located.
    //    STATUS_FILE_NOT_FOUND,
    //    //! An object could not be located.
    //    STATUS_OBJECT_NOT_FOUND,
    //    //! An invalid port was specified.
    //    STATUS_INVALID_PORT,
    //    //! An invalid selection was specified.
    //    STATUS_INVALID_SELECTION,
    //    //! An attempt was made to create a connection that resulted in a cycle.
    //    STATUS_CYCLE_DETECTED,
    //    //! An initial invalid status.
    //    STATUS_UNKNOWN
    void Status::configure(dagbase::ConfigurationElement &config)
    {
        ConfigurationElement::readConfig<StatusCode>(config, "statusCode", &parseStatusCode, &status);
        ConfigurationElement::readConfig<ResultType>(config, "resultType", &parseResultType, &resultType);
        switch (resultType)
        {
            case RESULT_NONE:
                break;
            case RESULT_NODE_ID:
            {
                NodeID value;
                ConfigurationElement::readConfig<NodeID>(config, "nodeID", &value);
                result.emplace(value);
                break;
            }
            case RESULT_PORT_ID:
            {
                PortID value;
                ConfigurationElement::readConfig<PortID>(config, "portID", &value);
                result.emplace(value);
                break;
            }
            case RESULT_SIGNAL_PATH_ID:
            {
                SignalPathID value;
                ConfigurationElement::readConfig<SignalPathID>(config, "signalPathID", &value);
                result.emplace(value);
                break;
            }
        }
    }

    Variant Status::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "statusCode", std::uint32_t{status});
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "resultType", std::uint32_t{resultType});
        if (retval.has_value())
            return retval;

        switch (resultType)
        {
            case RESULT_NONE:
                break;
            case RESULT_NODE_ID:
                retval = findEndpoint(path, "nodeID", std::get<NodeID>(result.value()));

                break;
            case RESULT_PORT_ID:
                retval = findEndpoint(path, "portID", std::get<PortID>(result.value()));

                break;
            case RESULT_SIGNAL_PATH_ID:
                retval = findEndpoint(path, "signalPathID", std::get<SignalPathID>(result.value()));

                break;
        }
        if (retval.has_value())
            return retval;

        return {};
    }

    std::ostream& Status::print() const
    {
        return std::cout << *this << std::endl;
    }

    const char* Status::statusCodeToString(Status::StatusCode value)
    {
        switch (value)
        {
            ENUM_NAME(STATUS_OK)
            ENUM_NAME(STATUS_FILE_NOT_FOUND)
            ENUM_NAME(STATUS_OBJECT_NOT_FOUND)
            ENUM_NAME(STATUS_INVALID_PORT)
            ENUM_NAME(STATUS_INVALID_SELECTION)
            ENUM_NAME(STATUS_CYCLE_DETECTED)
            ENUM_NAME(STATUS_SYNTAX_ERROR)
            ENUM_NAME(STATUS_FAILED_TO_CREATE_GRAPH)
            ENUM_NAME(STATUS_INTERNAL_ERROR)
            ENUM_NAME(STATUS_NO_UPSTREAM_CONNECTION)
            ENUM_NAME(STATUS_DUPLICATE_FUNCTION)
            ENUM_NAME(STATUS_UNKNOWN)
        }

        return "<error>";
    }

    Status::StatusCode Status::parseStatusCode(const char* str)
    {
        TEST_ENUM(STATUS_OK, str);
        TEST_ENUM(STATUS_FILE_NOT_FOUND, str);
        TEST_ENUM(STATUS_OBJECT_NOT_FOUND, str);
        TEST_ENUM(STATUS_INVALID_PORT, str);
        TEST_ENUM(STATUS_INVALID_SELECTION, str);
        TEST_ENUM(STATUS_CYCLE_DETECTED, str);
        TEST_ENUM(STATUS_SYNTAX_ERROR, str);
        TEST_ENUM(STATUS_FAILED_TO_CREATE_GRAPH, str);
        TEST_ENUM(STATUS_INTERNAL_ERROR, str);
        TEST_ENUM(STATUS_NO_UPSTREAM_CONNECTION, str);
        TEST_ENUM(STATUS_DUPLICATE_FUNCTION, str);
        TEST_ENUM(STATUS_UNKNOWN, str);

        return STATUS_UNKNOWN;
    }

    const char * Status::resultTypeToString(ResultType value)
    {
        switch (value)
        {
            ENUM_NAME(RESULT_NONE)
            ENUM_NAME(RESULT_NODE_ID)
            ENUM_NAME(RESULT_PORT_ID)
            ENUM_NAME(RESULT_SIGNAL_PATH_ID)
        }
        return "<error>";
    }

    Status::ResultType Status::parseResultType(const char *str)
    {
        TEST_ENUM(RESULT_NONE, str);
        TEST_ENUM(RESULT_NODE_ID, str);
        TEST_ENUM(RESULT_PORT_ID, str);
        TEST_ENUM(RESULT_SIGNAL_PATH_ID, str);

        return RESULT_NONE;
    }

    std::ostream & operator<<(std::ostream &ostr, const Status &value)
    {
        ostr << "Status { status: " << Status::statusCodeToString(value.status) << ", resultType " << Status::resultTypeToString(value.resultType) << " }";

        return ostr;
    }

}