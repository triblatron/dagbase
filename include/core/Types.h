//
// Created by tony on 04/02/24.
//

#pragma once

#include "config/DagBaseExport.h"
#include "util/VectorSet.h"
#include "util/SearchableSet.h"
#include "core/ConfigurationElement.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "MetaClassRegistration.h"
#include "util/Name.h"

#include <unordered_set>
#include <cstdint>
#include <string>
#include <iosfwd>


namespace dagbase
{
    class ConfigurationElement;
    class Graph;
    class Node;
    class Port;
    class SignalPath;
    class Template;
    class Value;

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

//! Macro to declare a ID type
//! This ensures type safety because IDs will have different types.
//! They therefore cannot be mixed.
#define INF_ID_DECLARE(Ident, Tag) \
    using Ident = Name<Tag>

    INF_ID_DECLARE(NodeID, Node);
    INF_ID_DECLARE(PortID, Port);
    INF_ID_DECLARE(TemplateID, Template);
    INF_ID_DECLARE(SignalPathID, SignalPath);
    INF_ID_DECLARE(ValueID, Value);

    static_assert(std::is_standard_layout_v<Name<Value>>);   // the STL itself

    struct CompareNodesById;
    typedef SearchableSet<VectorSet<Node*, CompareNodesById>> NodeSet;

    enum CopyOp : std::uint32_t
    {
        DEEP_COPY_NONE          = 0,
        DEEP_COPY_NODES_BIT     = 1<<0,
        DEEP_COPY_INPUTS_BIT    = 1<<1,
        DEEP_COPY_OUTPUTS_BIT   = 1<<2,
        GENERATE_UNIQUE_ID_BIT  = 1<<3,
        DEEP_COPY_PARENT_BIT    = 1<<4,
        ADD_CHILD_GRAPHS_BIT    = 1<<5
    };

    std::string DAGBASE_API copyOpToString(CopyOp op);

    CopyOp DAGBASE_API parseCopyOp(std::string_view str);

    enum ComparisonFlags : std::uint32_t
    {
        CMP_NONE,
        CMP_IDENT_BIT = 1U<<0U,
        CMP_NAME_BIT = 1U<<1U,
        CMP_CONNECTIONS_IDENT_BIT = 1U<<3U,
        CMP_CONNECTIONS_COUNT_BIT = 1U<<4U,
        CMP_CONNECTIONS_BIT = CMP_CONNECTIONS_IDENT_BIT|CMP_CONNECTIONS_COUNT_BIT,
    };

    std::string DAGBASE_API comparisonFlagsToString(dagbase::ComparisonFlags value);

    dagbase::ComparisonFlags DAGBASE_API parseComparisonFlags(const std::string& str);

    enum TestEnum : std::uint32_t
    {
        TEST_UNKNOWN,
        TEST_FOO,
        TEST_BAR,
        TEST_BAZ
    };

    const char DAGBASE_API * testToString(TestEnum value);

    TestEnum DAGBASE_API parseTest(const char* str);

    DAGBASE_REGISTER_ENUM(TestEnumWrapper, TestEnum, testToString, parseTest, TEST_UNKNOWN, TEST_FOO, TEST_BAZ)
};
