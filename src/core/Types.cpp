#include "config/config.h"

#include "core/Types.h"
#include "util/enums.h"
#include "core/ConfigurationElement.h"

#include <cstring>
#include <iostream>

namespace dagbase
{
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
        BIT_NAME(op, ADD_CHILD_GRAPHS_BIT, str);

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
        TEST_BIT(ADD_CHILD_GRAPHS_BIT, str, value);

        return value;
    }

    std::string comparisonFlagsToString(ComparisonFlags value)
    {
        std::string retval;

        if (value == CMP_NONE)
            return "CMP_NONE";

        BIT_NAME(value, CMP_IDENT_BIT, retval);
        BIT_NAME(value, CMP_NAME_BIT, retval);
        // Handle an enum which is a combination of other enums so that it resolves correctly without duplication
        if ((value & CMP_CONNECTIONS_BIT) == CMP_CONNECTIONS_BIT)
        {
            BIT_NAME(value, CMP_CONNECTIONS_BIT, retval);
        }
        else
        {
            BIT_NAME(value, CMP_CONNECTIONS_COUNT_BIT, retval);
            BIT_NAME(value, CMP_CONNECTIONS_IDENT_BIT, retval);
        }

        if (!retval.empty() && retval.back() == ' ')
            retval.pop_back();

        return retval;
    }

    ComparisonFlags parseComparisonFlags(const std::string& str)
    {
        ComparisonFlags value{CMP_NONE};

        TEST_BIT(CMP_IDENT_BIT, str, value);
        TEST_BIT(CMP_NAME_BIT, str, value);
        TEST_BIT(CMP_CONNECTIONS_BIT, str, value);
        TEST_BIT(CMP_CONNECTIONS_COUNT_BIT, str, value);
        TEST_BIT(CMP_CONNECTIONS_IDENT_BIT, str, value);

        return value;
    }

    const char * testToString(TestEnum value)
    {
        switch (value)
        {
            ENUM_NAME(TEST_UNKNOWN)
            ENUM_NAME(TEST_FOO)
            ENUM_NAME(TEST_BAR)
            ENUM_NAME(TEST_BAZ)
        }

        return "<error>";
    }

    TestEnum parseTest(const char *str)
    {
        TEST_ENUM(TEST_FOO, str);
        TEST_ENUM(TEST_BAR, str);
        TEST_ENUM(TEST_BAZ, str);

        return TEST_UNKNOWN;
    }
}
