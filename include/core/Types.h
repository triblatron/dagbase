//
// Created by tony on 04/02/24.
//

#pragma once

#include "config/DagBaseExport.h"
#include "util/VectorSet.h"
#include "util/SearchableSet.h"
#include "core/ConfigurationElement.h"

#include <unordered_set>
#include <cstdint>
#include <string>


namespace dagbase
{
    class ConfigurationElement;
    class Graph;
    class Node;
    class Port;

    //! The direction of a port.
    class DAGBASE_API PortDirection
    {
    public:
        enum Direction
        {
            //! Initial or invalid port direction.
            DIR_UNKNOWN,
            //! Input port.
            DIR_IN,
            //! Output port.
            DIR_OUT,
            //! Internal port, cannot be connected.
            DIR_INTERNAL
        };

        //! Parse a string representation of a port direction.
        static Direction parseFromString(const char* str);

        //! Convert a port direction to a string.
        static const char* toString(Direction dir);
    };

    class DAGBASE_API PortType
    {
    public:
        enum Type
        {
            TYPE_UINT8,
            TYPE_INT8,
            TYPE_UINT16,
            TYPE_INT16,
            TYPE_UINT32,
            TYPE_INT32,
            TYPE_UINT64,
            //! A std::int64_t
            TYPE_INT64,
            TYPE_FLOAT,
            //! A double-precision floating point number.
            TYPE_DOUBLE,
            //! A std::string
            TYPE_STRING,
            //! A bool
            TYPE_BOOL,
            //! An osg::Vec3d
            TYPE_VEC3D,
            //! An opaque pointer (void*).
            TYPE_OPAQUE,
            //! A std::vector of values.
            TYPE_VECTOR,
            //! Initial or invalid type.
            TYPE_UNKNOWN
        };

        //! Parse a string representation of a port type.
        static Type parseFromString(const char* str);

        //! Convert a port type to a string.
        static const char* toString(Type type);

        static const char* className(Type type);
    };

//! Macro to declare a ID type
//! This ensures type safety because IDs will have different types.
//! They therefore cannot be mixed.
#define INF_ID_DECLARE(Name) \
    class Name               \
    {                        \
    public:                  \
        enum                 \
        {                    \
            INVALID_ID = -1  \
        };                   \
    public:                  \
        Name()               \
        :                    \
        id(INVALID_ID)       \
        {                    \
        }                    \
        \
        Name(std::int64_t id)\
        :                    \
        id(id)               \
        {                    \
        }                    \
                             \
        Name(const Name& other) = default; \
        Name& operator=(const Name& other) \
        {                    \
            if (this != &other) \
            {                \
                id = other.id;  \
            }               \
            return *this;\
        }                    \
                             \
        Name& operator=(std::int64_t id_)   \
        {                    \
            this->id = id_;   \
                             \
            return *this;    \
        }                    \
                             \
        const Name operator++(int) \
        {                    \
            Name old = *this;\
            id++;            \
                             \
            return old;      \
        }                    \
                             \
        operator std::int64_t() const       \
        {                    \
            return id;\
        }                    \
                             \
        void configure(dagbase::ConfigurationElement& config)                     \
        {\
            id = config.asInteger();\
        }\
        \
        bool valid() const   \
        {                    \
            return id!=-1;   \
        }\
    private:             \
        std::int64_t id{INVALID_ID};     \
                             \
    }
    INF_ID_DECLARE(NodeID);
    INF_ID_DECLARE(PortID);
    INF_ID_DECLARE(TemplateID);
    INF_ID_DECLARE(SignalPathID);

    //! A return value from an RPC.
    struct DAGBASE_API Status
    {
        enum StatusCode
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
            //! An initial invalid status.
            STATUS_UNKNOWN
        };

        StatusCode status{STATUS_UNKNOWN};
        enum ResultType
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

        void configure(dagbase::ConfigurationElement& config);

        Variant find(std::string_view path) const;

        static const char* statusCodeToString(StatusCode value);

        static StatusCode parseStatusCode(const char* str);

        static const char* resultTypeToString(ResultType value);

        static ResultType parseResultType(const char* str);
    };

    struct CompareNodesById;
    typedef SearchableSet<VectorSet<Node*, CompareNodesById>> NodeSet;

    enum CopyOp : std::uint32_t
    {
        DEEP_COPY_NONE          = 0,
        DEEP_COPY_NODES_BIT     = 1<<0,
        DEEP_COPY_INPUTS_BIT    = 1<<1,
        DEEP_COPY_OUTPUTS_BIT   = 1<<2,
        GENERATE_UNIQUE_ID_BIT  = 1<<3,
        DEEP_COPY_PARENT_BIT    = 1<<4
    };

    std::string DAGBASE_API copyOpToString(CopyOp op);

    CopyOp DAGBASE_API parseCopyOp(std::string_view str);
};
