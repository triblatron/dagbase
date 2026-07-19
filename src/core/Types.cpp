#include "config/config.h"

#include "core/Types.h"
#include "util/enums.h"
#include "core/ConfigurationElement.h"

#include <cstring>

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
            ENUM_NAME(STATUS_UNKNOWN)
        }

        return "<error>";
    }

    Status::StatusCode Status::parseStatusCode(const char* value)
    {
        TEST_ENUM(STATUS_OK, value);
        TEST_ENUM(STATUS_FILE_NOT_FOUND, value);
        TEST_ENUM(STATUS_OBJECT_NOT_FOUND, value);
        TEST_ENUM(STATUS_INVALID_PORT, value);
        TEST_ENUM(STATUS_INVALID_SELECTION, value);
        TEST_ENUM(STATUS_CYCLE_DETECTED, value);
        TEST_ENUM(STATUS_SYNTAX_ERROR, value);
        TEST_ENUM(STATUS_FAILED_TO_CREATE_GRAPH, value);
        TEST_ENUM(STATUS_INTERNAL_ERROR, value);
        TEST_ENUM(STATUS_UNKNOWN, value);

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

    PortType::Type PortType::parseFromString(const char* str)
	{
	    TEST_ENUM(TYPE_UINT8, str);
	    TEST_ENUM(TYPE_INT8, str);
	    TEST_ENUM(TYPE_UINT16, str);
	    TEST_ENUM(TYPE_INT16, str);
	    TEST_ENUM(TYPE_UINT32, str);
	    TEST_ENUM(TYPE_INT32, str);
	    TEST_ENUM(TYPE_UINT64, str);
	    TEST_ENUM(TYPE_INT64, str);
	    TEST_ENUM(TYPE_FLOAT, str);
	    TEST_ENUM(TYPE_DOUBLE, str);
	    TEST_ENUM(TYPE_STRING, str);
	    TEST_ENUM(TYPE_BOOL, str);
	    TEST_ENUM(TYPE_VEC3D, str);
	    TEST_ENUM(TYPE_OPAQUE, str);
	    TEST_ENUM(TYPE_VECTOR, str);
	    TEST_ENUM(TYPE_UNKNOWN, str);

		return TYPE_UNKNOWN;
	}

    const char *PortType::toString(Type type)
    {
        switch (type)
        {
            ENUM_NAME(TYPE_UINT8)
            ENUM_NAME(TYPE_INT8)
            ENUM_NAME(TYPE_UINT16)
            ENUM_NAME(TYPE_INT16)
            ENUM_NAME(TYPE_UINT32)
            ENUM_NAME(TYPE_INT32)
            ENUM_NAME(TYPE_UINT64)
            ENUM_NAME(TYPE_INT64)
            ENUM_NAME(TYPE_FLOAT)
            ENUM_NAME(TYPE_DOUBLE)
            ENUM_NAME(TYPE_STRING)
            ENUM_NAME(TYPE_BOOL)
            ENUM_NAME(TYPE_VEC3D)
            ENUM_NAME(TYPE_OPAQUE)
            ENUM_NAME(TYPE_VECTOR)
            ENUM_NAME(TYPE_UNKNOWN)
        }

        return "<error>";
    }

    const char* PortType::className(Type type)
    {
        switch (type)
        {
            ENUM_ALT_NAME(PortType::TYPE_UINT8, "TypedPort<uint8_t>")
            ENUM_ALT_NAME(PortType::TYPE_INT8, "TypedPort<int8_t>")
            ENUM_ALT_NAME(PortType::TYPE_UINT16, "TypedPort<uint16_t>")
            ENUM_ALT_NAME(PortType::TYPE_INT16, "TypedPort<int16_t>")
            ENUM_ALT_NAME(PortType::TYPE_UINT32, "TypedPort<uint32_t>")
            ENUM_ALT_NAME(PortType::TYPE_INT32, "TypedPort<int32_t>")
            ENUM_ALT_NAME(PortType::TYPE_UINT64, "TypedPort<uint64_t>")
            ENUM_ALT_NAME(PortType::TYPE_INT64, "TypedPort<int64_t>")
            ENUM_ALT_NAME(PortType::TYPE_FLOAT, "TypedPort<float>")
            ENUM_ALT_NAME(PortType::TYPE_DOUBLE, "TypedPort<double>")
            ENUM_ALT_NAME(PortType::TYPE_STRING, "TypedPort<string>")
            ENUM_ALT_NAME(PortType::TYPE_BOOL, "TypedPort<bool>")
            ENUM_ALT_NAME(PortType::TYPE_OPAQUE, "TypedPort<void*>")
        }

        return "<error>";
    }

    std::string copyOpToString(CopyOp op)
    {
        std::string str;

        if (op == DEEP_COPY_NONE)
            return "DEEP_COPY_NONE";

        BIT_NAME(op, DEEP_COPY_NODES_BIT, str);
        BIT_NAME(op, DEEP_COPY_INPUTS_BIT, str);
        BIT_NAME(op, DEEP_COPY_OUTPUTS_BIT, str);
        BIT_NAME(op, GENERATE_UNIQUE_ID_BIT, str);
        BIT_NAME(op, DEEP_COPY_PARENT_BIT, str);

        if (!str.empty() && str.back() == ' ')
            str.pop_back();

        return str;
    }

    CopyOp parseCopyOp(std::string_view str)
    {
        CopyOp value{DEEP_COPY_NONE};

        TEST_BIT(DEEP_COPY_NODES_BIT, str, value);
        TEST_BIT(DEEP_COPY_INPUTS_BIT, str, value);
        TEST_BIT(DEEP_COPY_OUTPUTS_BIT, str, value);
        TEST_BIT(GENERATE_UNIQUE_ID_BIT, str, value);
        TEST_BIT(DEEP_COPY_PARENT_BIT, str, value);

        return value;
    }

    std::string comparisonFlagsToString(ComparisonFlags value)
    {
        std::string retval;

        if (value == CMP_NONE)
            return "CMP_NONE";

        BIT_NAME(value, CMP_IDENT_BIT, retval);
        BIT_NAME(value, CMP_NAME_BIT, retval);

        if (!retval.empty() && retval.back() == ' ')
            retval.pop_back();

        return retval;
    }

    ComparisonFlags parseComparisonFlags(const std::string& str)
    {
        ComparisonFlags value{CMP_NONE};

        TEST_BIT(CMP_IDENT_BIT, str, value);
        TEST_BIT(CMP_NAME_BIT, str, value);

        return value;
    }

    PortDirection::Direction PortDirection::parseFromString(const char* str)
	{
		Direction dir = DIR_UNKNOWN;

		if (std::strcmp(str, "DIR_IN") == 0)
		{
			dir = DIR_IN;
		}
		else if (std::strcmp(str, "DIR_OUT") == 0)
		{
			dir = DIR_OUT;
		}
		else if (std::strcmp(str, "DIR_INTERNAL") == 0)
		{
			dir = DIR_INTERNAL;
		}

		return dir;
	}

    static const char* directionNames[]=
    {
        "DIR_UNKNOWN",
        "DIR_IN",
        "DIR_OUT",
        "DIR_INTERNAL"
    };

    const char *PortDirection::toString(PortDirection::Direction dir)
    {
        return directionNames[dir];
    }
}
