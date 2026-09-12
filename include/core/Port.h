#pragma once

#include "config/DagBaseExport.h"

#include "core/Class.h"

#include "Value.h"
#include "Types.h"
#include "NodeLibrary.h"
#include "MetaPort.h"

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <string_view>

namespace dagbase
{
    class InputStream;
    class OutputStream;
    class CloningFacility;
    class DebugPrinter;
    class KeyGenerator;
    struct MetaPort;
    class Node;

    class DAGBASE_API ValueVisitor
    {
    public:
        void setInt(std::int64_t value)
        {
            _value = value;
        }

        void setDouble(double value)
        {
            _value = value;
        }

        void setString(std::string value)
        {
            _value = std::move(value);
        }

        void setBool(bool value)
        {
            _value = value;
        }

        void setValue(const Value& value)
        {
            _value = (value);
        }

        template<typename T>
        void setValue(T value)
        {
            _value = value;
        }

        [[nodiscard]]Value value() const
        {
            return _value;
        }
    private:
        Value _value;
    };

    class SetValueVisitor
    {
    public:
        explicit SetValueVisitor(Value value)
        :
        _value(std::move(value))
        {
            // Do nothing.
        }

        [[nodiscard]]Value value() const
        {
            return _value;
        }
    private:
        Value _value;
    };

	class DAGBASE_API Port : public dagbase::Class
    {
    public:
        typedef std::vector<Port*> PortArray;

        enum PortFlags : std::uint32_t
        {
            FLAGS_NONE          = 0,
            OWN_META_PORT_BIT   = (1U<<0U),
            OWN_INPUTS_BIT      = (1U<<1U),
            OWN_OUTPUTS_BIT     = (1U<<2U),
            REMOVED_BIT         = (1U<<3U)
        };
    public:
	    Port() = default;

        Port(PortID id, Node* parent, std::string name, PortType::Type type, PortDirection::Direction dir, PortFlags flags=FLAGS_NONE, Value value=Value());

        Port(const Port &port, CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen);

        Port(Port &&port) = default;

        //! Construct from a stream and a node library.
        //! \note Requires the NodeLibrary to read the parent.
        Port(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& lua);

	    dagbase::InputStream& readFromStream(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& lua) override;

	    ~Port() override;

        bool operator==(const Port& other) const;
        
        void setId(PortID id)
        {
            _id = id;
        }

        [[nodiscard]]PortID id() const
        {
            return _id;
        }

        [[nodiscard]] const std::string &name() const
        {
            return _name;
        }

        [[nodiscard]] PortType::Type type() const
        {
            return _type;
        }

        void setDir(PortDirection::Direction dir)
        {
            _direction = dir;
        }

        [[nodiscard]] PortDirection::Direction dir() const
        {
            return _direction;
        }

	    void setFlags(PortFlags flags)
        {
            _flags = flags;
        }

	    PortFlags flags() const
        {
            return _flags;
        }

	    [[nodiscard]] size_t numOutgoingConnections() const;

	    void addOutgoingConnection(Port* input, Graph* parent, KeyGenerator& keyGen);

	    void replaceOutgoingConnection(Port* needle, Port* replacement);

	    [[nodiscard]] size_t numIncomingConnections() const;

	    void addIncomingConnection(Port* output, Graph* parent, KeyGenerator& keyGen);

	    void replaceIncomingConnection(Port* needle, Port* replacement);

	    void markRemoved()
        {
            _flags = static_cast<PortFlags>(_flags | REMOVED_BIT);
        }

	    void unmarkRemoved()
        {
            _flags = static_cast<PortFlags>(_flags & ~REMOVED_BIT);
        }

	    bool isMarkedRemoved() const
        {
            return (_flags & REMOVED_BIT) != 0;
        }

		[[nodiscard]]bool isCompatibleWith(const Port& other) const
		{
			return ((type() == other.type()));// || (type() == TYPE_INTEGER && other.type() == TYPE_DOUBLE) || (type() == TYPE_BOOL && other.type() == TYPE_INTEGER) || (type() == TYPE_BOOL && other.type() == TYPE_DOUBLE));
		}

        void setParent(Node* parent)
        {
            _parent = parent;
        }

        [[nodiscard]]Node* parent()
        {
            return _parent;
        }

        const Node* parent() const
        {
            return _parent;
        }

	    void setSharedParent(Node* parent)
        {
            _sharedParent = parent;
        }

	    Node* sharedParent()
        {
            return _sharedParent;
        }

	    const Node* sharedParent() const
        {
            return _sharedParent;
        }

	    void reconnectTo(NodeSet const& selection, Node* newDest, KeyGenerator& keyGen);

	    void reconnectFrom(NodeSet const& selection, Node* newSource, KeyGenerator& keyGen);

        Port* clone(CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen) const
        {
            return new Port(*this, facility, copyOp, keyGen);
        }

        dagbase::OutputStream& writeToStream(dagbase::OutputStream& str, NodeLibrary& nodeLib, Lua& lua) const override;

        [[nodiscard]]virtual bool equals(const Port& other, ComparisonFlags flags) const
        {
            if ((flags & CMP_IDENT_BIT) != 0)
            {
                if (_id != other._id)
                    return false;
            }

            if ((flags & CMP_NAME_BIT) != 0)
            {
                if (_name != other._name)
                    return false;
            }

            if ((flags & CMP_CONNECTIONS_COUNT_BIT) != 0)
            {
                auto numIncomingThis = numIncomingConnections();
                auto numIncomingOther = other.numIncomingConnections();
                if (numIncomingThis != numIncomingOther)
                    return false;

                if (numOutgoingConnections() != other.numOutgoingConnections())
                    return false;
            }

            if ((flags & CMP_CONNECTIONS_IDENT_BIT)!=0)
            {

            }
            return *this == other;
        }

        virtual void debug(dagbase::DebugPrinter& printer) const;

        virtual DebugPrinter &toLua(DebugPrinter &printer) const;

        [[nodiscard]]const char* className() const override
        {
            return "Port";
        }

	    const Value& value() const
        {
            return _value;
        }

	    void setValue(const Value& value)
        {
            _value = value;
        }

	    Value& value()
        {
            return _value;
        }

        Variant find(std::string_view path) const;

	    static std::string portFlagsToString(PortFlags flags);

	    static PortFlags parsePortFlags(const std::string& str);
    protected:
        void setFlag(PortFlags mask)
        {
            _flags = static_cast<PortFlags>(_flags | mask);
        }

        void setOwnMetaPort(bool own)
        {
            if (own)
            {
                _flags = static_cast<PortFlags>(_flags | OWN_META_PORT_BIT);
            }
            else
            {
                _flags = static_cast<PortFlags>(_flags & ~OWN_META_PORT_BIT);
            }
        }

        [[nodiscard]]bool ownMetaPort() const
        {
            return (_flags & OWN_META_PORT_BIT) != 0x0;
        }
	private:
        std::string _name;
	    PortType::Type _type{PortType::TYPE_UNKNOWN};
	    PortDirection::Direction _direction{PortDirection::DIR_UNKNOWN};
        PortID _id{ 0 };
        Node* _parent{nullptr};
	    Node* _sharedParent{nullptr};
        PortFlags _flags{FLAGS_NONE};
	    Value _value;
	};
}
