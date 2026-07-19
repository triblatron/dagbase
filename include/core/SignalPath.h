//
// Created by tony on 18/02/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"
#include "core/Port.h"
#include "core/Variant.h"

#include <string_view>
#include <iosfwd>

namespace dagbase
{
    class InputStream;
    class NodeLibrary;
    class OutputStream;

    class DAGBASE_API SignalPath
    {
    public:
        enum Flags : std::uint32_t
        {
            FLAGS_NONE = 0x0,
            REMOVED_BIT = 1<<0
        };
    public:
        SignalPath(KeyGenerator& keyGen, Port* source, Port* dest)
        :
        _id(keyGen.nextSignalPathID()),
        _source(source),
        _dest(dest)
        {
            // Do nothing.
        }

        SignalPath(SignalPathID id, Port* source, Port* dest);

        SignalPath(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& lua);

        bool equals(const SignalPath& other, ComparisonFlags flags) const;

        [[nodiscard]]SignalPathID id() const
        {
            return _id;
        }

        void setSource(Port* source)
        {
            _source = source;
        }

        Port* source()
        {
            return _source;
        }

        const Port* source() const
        {
            return _source;
        }

        void setDest(Port* dest)
        {
            _dest = dest;
        }

        Port* dest()
        {
            return _dest;
        }

        const Port* dest() const
        {
            return _dest;
        }

        std::size_t sourceIndex() const;

        [[nodiscard]]Node* sourceNode()
        {
            if (_source!=nullptr)
            {
                return _source->parent();
            }

            return nullptr;
        }

        [[nodiscard]]const Node* sourceNode() const
        {
            if (_source!=nullptr)
            {
                return _source->parent();
            }

            return nullptr;
        }

        std::size_t destIndex() const;

        [[nodiscard]]Node* destNode()
        {
            if (_dest!=nullptr)
            {
                return _dest->parent();
            }

            return nullptr;
        }

        [[nodiscard]]const Node* destNode() const
        {
            if (_dest!=nullptr)
            {
                return _dest->parent();
            }

            return nullptr;
        }

        void markRemoved()
        {
            _flags = static_cast<Flags>(_flags | REMOVED_BIT);
        }

        void clearRemoved()
        {
            _flags = static_cast<Flags>(_flags &~REMOVED_BIT);
        }

        [[nodiscard]]bool isRemoved() const
        {
            return (_flags & REMOVED_BIT)!=0x0;
        }

        DebugPrinter &toLua(DebugPrinter &printer);

        dagbase::OutputStream& writeToStream(dagbase::OutputStream& str, NodeLibrary& nodeLib, Lua& lua) const;

        dagbase::Variant find(std::string_view path) const;
    private:
        SignalPathID _id;
        Port* _source{nullptr};
        Port* _dest{nullptr};
        Flags _flags{FLAGS_NONE};
    };

    std::ostream DAGBASE_API& operator<<(std::ostream& str, const dagbase::SignalPath& obj);
}
