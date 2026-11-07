#pragma once

#include "config/DagBaseExport.h"

#include "core/Action.h"

#include <cstdint>

#include "core/MetaProperty.h"

namespace dagbase
{
    //! A base class for commands using the Command pattern
    //! This is effectively a kind of node but does not inherit Node
    //! An implementation must be default-constructible for a
    //! common interface.  It must also support setting arguments
    //! via a generic interface.  This does not have to be
    //! as efficient as for a Node, so setting arguments by
    //! name is desirable.
    class DAGBASE_API Command : public Action
    {
    public:
        enum Flags : std::uint32_t
        {
            CMD_NONE,
            CMD_COMPLETE = 1<<0
        };
    public:
        ~Command() override = default;

        //! Reset state ready for reusing this object.
        virtual void reset();

        virtual Command * clone() const = 0;

        virtual bool isUndoable() const
        {
            return true;
        }

        void setFlag(Flags flag)
        {
            _flags = static_cast<Flags>(_flags | flag);
        }

        void clearFlag(Flags flag)
        {
            _flags = static_cast<Flags>(_flags & ~flag);
        }

        bool isFlagSet(Flags flag) const
        {
            return (_flags & flag) != 0;
        }

        bool isComplete() const
        {
            return isFlagSet(CMD_COMPLETE);
        }

        //! Use stored state to undo a call to makeItSo().
        virtual void undo() = 0;

        //! Provides default implementation that just calls makeItSo().
        virtual void redo() = 0;

        static std::string flagsToString(Flags flags);

        static Flags parseFlags(const char* str);
    protected:
        void markComplete()
        {
            setFlag(CMD_COMPLETE);
        }
    private:
        Flags _flags{CMD_NONE};
    };
}
