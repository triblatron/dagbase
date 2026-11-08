#include "config/config.h"

#include "core/Command.h"

#include "util/enums.h"

namespace dagbase
{
    Command::Command(const Command &other)
    {
        _flags = other._flags;
    }

    void Command::reset()
    {
        clearFlag(CMD_COMPLETE);
    }

    std::string Command::flagsToString(Flags flags)
    {
        std::string retval;

        if (flags == CMD_NONE)
            return "CMD_NONE";

        BIT_NAME(flags, CMD_COMPLETE, retval)
        BIT_NAME(flags, CMD_UNDOABLE_BIT, retval)
        BIT_NAME(flags, CMD_LONG_RUNNING_BIT, retval)

        if (!retval.empty() && retval.back() == ' ')
            retval.pop_back();

        return retval;
    }

    Command::Flags Command::parseFlags(const char *str)
    {
        Flags retval{CMD_NONE};

        TEST_BIT_CSTR(CMD_COMPLETE, str, retval)
        TEST_BIT_CSTR(CMD_UNDOABLE_BIT, str, retval)
        TEST_BIT_CSTR(CMD_LONG_RUNNING_BIT, str, retval)

        return retval;
    }
}
