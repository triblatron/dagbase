//
// Created by tony on 18/02/24.
//
#include "config/config.h"

#include "core/SignalPath.h"
#include "io/InputStream.h"
#include "../../include/core/TypedPort.h"
#include "core/NodeLibrary.h"
#include "io/OutputStream.h"
#include "core/Node.h"

namespace dagbase
{
    std::ostream & operator<<(std::ostream &str, const dagbase::SignalPath &obj)
    {
        str << "SignalPath { id: " << obj.id() << ", source: " << obj.source()->id() << ", dest: " << obj.dest()->id() << " }";

        return str;
    }

    dagbase::OutputStream &SignalPath::writeToStream(dagbase::OutputStream &str, NodeLibrary& nodeLib, Lua& lua) const
    {
        str.writeHeader("SignalPath");
        str.writeField("id");
        str.writeInt64(_id);
        str.writeField("source");
        if (str.writeRef(_source))
        {
            _source->writeToStream(str, nodeLib, lua);
        }
        str.writeField("dest");
        if (str.writeRef(_dest))
        {
            _dest->writeToStream(str, nodeLib, lua);
        }
        str.writeField("flags");
        str.writeUInt32(_flags);
        str.writeFooter();

        return str;
    }

    dagbase::Variant SignalPath::find(std::string_view path) const
    {
        Variant retval;

        if (_source)
        {
            retval = findEndpoint(path, "fromPort", _source->id());
            if (retval.has_value())
                return retval;
        }

        if (_dest)
        {
            retval = findEndpoint(path, "toPort", _dest->id());
            if (retval.has_value())
                return retval;
        }

        return {};
    }


    SignalPath::SignalPath(SignalPathID id, Port *source, Port *dest)
    :
    _id(id),
    _source(source),
    _dest(dest)
    {
        // Do nothing.§
    }

    SignalPath::SignalPath(dagbase::InputStream &str, NodeLibrary& nodeLib, dagbase::Lua& lua)
    :
    _source(nullptr),
    _dest(nullptr),
    _flags(FLAGS_NONE)
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);
        str.readField(&fieldName);
        std::int64_t id{0};
        str.read(&id);
        _id = id;
        str.readField(&fieldName);
        _source = str.readRef<Port>("Port", nodeLib, lua);
        str.readField(&fieldName);
        _dest = str.readRef<Port>("Port", nodeLib, lua);
        str.readField(&fieldName);
        std::uint32_t flags{0};
        str.readUInt32(&flags);
        _flags = static_cast<Flags>(flags);
        str.readFooter();
    }

    bool SignalPath::equals(const SignalPath &other, ComparisonFlags flags) const
    {
        if (this == &other)
            return true;

        if ((flags & CMP_IDENT_BIT)!=0)
        {
            if (_id != other._id)
                return false;
        }

        if (_source && !other._source)
            return false;

        if (!_source && other._source)
            return false;

        // The unique identifier does not participate in equality.
        if (_source && !_source->equals(*other._source, flags))
        {
            return false;
        }

        if (_dest && !other._dest)
            return false;

        if (!_dest && other._dest)
            return false;

        if (_dest && !_dest->equals(*other._dest, flags))
        {
            return false;
        }

        return true;
    }

    std::size_t SignalPath::sourceIndex() const
    {
        if (_source)
        {
            return _source->parent()->indexOfPort(_source);
        }
        else
        {
            return std::numeric_limits<std::size_t>::max();
        }
    }

    std::size_t SignalPath::destIndex() const
    {
        if (_dest)
        {
            return _dest->parent()->indexOfPort(_dest);
        }
        else
        {
            return std::numeric_limits<std::size_t>::max();
        }
    }

    DebugPrinter &SignalPath::toLua(DebugPrinter &printer)
    {
        printer.println("{");
        printer.indent();
        printer.printIndent().print("id = ").print(_id).print(",\n");
        printer.printIndent().print("sourceNode = ").print(_source->parent()->id()).print(",\n");
        printer.printIndent().print("sourcePort = ").print(_source->id()).print(", \n");
        printer.printIndent().print("destNode = ").print(_dest->parent()->id()).print(",\n");
        printer.printIndent().print("destPort = ").print(_dest->id()).print(",\n");
        printer.outdent();
        printer.println("}");

        return printer;
    }
}
