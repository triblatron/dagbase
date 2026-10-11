//
// Created by Tony Horrobin on 11/10/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"

namespace dagbase
{
    class Graph;

    struct DAGBASE_API Status
    {
        enum StatusCode : std::uint32_t
        {
            //! Success
            STATUS_OK,
            //! A file could not be located.
            STATUS_FILE_NOT_FOUND,
            //! An object could not be located.
            STATUS_OBJECT_NOT_FOUND,
            //! An invalid port was specified.
            STATUS_INVALID_PORT,
            //! An invalid selection was specified.
            STATUS_INVALID_SELECTION,
            //! An attempt was made to create a connection that resulted in a cycle.
            STATUS_CYCLE_DETECTED,
            //! A syntax error occurred parsing a format, typically Lua
            STATUS_SYNTAX_ERROR,
            //! We failed to create a Graph
            STATUS_FAILED_TO_CREATE_GRAPH,
            //! Something went wrong in our internal structures
            STATUS_INTERNAL_ERROR,
            //! An input has no upstream connection, typically during topological sort
            STATUS_NO_UPSTREAM_CONNECTION,
            //! An initial invalid status.
            STATUS_UNKNOWN
        };

        StatusCode status{STATUS_UNKNOWN};
        enum ResultType : std::uint32_t
        {
            RESULT_NONE,
            RESULT_NODE_ID,
            RESULT_PORT_ID,
            RESULT_SIGNAL_PATH_ID
        };
        ResultType resultType{RESULT_NONE};
        typedef std::optional<std::variant<Graph*, NodeID, PortID, TemplateID, SignalPathID>> Result;
        Result result;

        Status() = default;

        explicit Status(StatusCode code)
        :
        status(code)
        {
            // Do nothing.
        }

        Status(const Status& other) = default;

        Status& operator=(const Status& other) = default;

        bool operator==(const Status& other) const
        {
            return status == other.status && resultType == other.resultType && result == other.result;
        }
        void configure(dagbase::ConfigurationElement& config);

        Variant find(std::string_view path) const;

        std::ostream& print() const;

        static const char* statusCodeToString(StatusCode value);

        static StatusCode parseStatusCode(const char* str);

        static const char* resultTypeToString(ResultType value);

        static ResultType parseResultType(const char* str);
    };
    std::ostream DAGBASE_API & operator<<(std::ostream& ostr, const Status& value);

}