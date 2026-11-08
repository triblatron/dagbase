#pragma once

#include "config/DagBaseExport.h"

#include "core/Action.h"

#include <cstdint>

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
            CMD_COMPLETE = 1<<0,
            CMD_UNDOABLE_BIT = 1<<1,
            CMD_LONG_RUNNING_BIT = 1<<2
        };
    public:
        Command() = default;

        Command(const Command& other);

        ~Command() override = default;

        bool isSameType(const Command& other) const
        {
            return typeid(*this) == typeid(other);
        }

        //! Reset state ready for reusing this object.
        virtual void reset();

        //! Support the Prototype pattern.
        //! \note Typically delegates to the copy ctor.
        virtual Command * clone() const = 0;

        //! \return whether this Command supports undo, typically because it is free of side effects.
        //! \note If this returns false, the undo stack should be cleared prior to executing this Command.
        bool isUndoable() const
        {
            return isFlagSet(CMD_UNDOABLE_BIT);
        }

        //! Set one or more flags
        void setFlag(Flags flag)
        {
            _flags = static_cast<Flags>(_flags | flag);
        }

        //! Clear one or more flags.
        void clearFlag(Flags flag)
        {
            _flags = static_cast<Flags>(_flags & ~flag);
        }

        //! \return whether one or more flags are set.
        bool isFlagSet(Flags flag) const
        {
            return (_flags & flag) != 0;
        }

        //! \return whether this Command has finished processing
        bool isComplete() const
        {
            return isFlagSet(CMD_COMPLETE);
        }

        bool isLongRunning() const
        {
            return isFlagSet(CMD_LONG_RUNNING_BIT);
        }

        virtual dagbase::Status step()
        {
            // Do nothing by default
            return dagbase::Status{dagbase::Status::STATUS_OK};
        }

        //! Use stored state to undo a call to makeItSo().
        virtual dagbase::Status undo() = 0;

        //! Provides default implementation that just calls makeItSo().
        virtual dagbase::Status redo(std::pmr::vector<dagbase::Variant>* results) = 0;

        static std::string flagsToString(Flags flags);

        static Flags parseFlags(const char* str);
    protected:
        void markComplete()
        {
            setFlag(CMD_COMPLETE);
        }
    private:
        Flags _flags{CMD_UNDOABLE_BIT};
    };
}
