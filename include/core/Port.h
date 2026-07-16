#pragma once

#include "config/DagBaseExport.h"

#include "core/Class.h"

#include "Value.h"
#include "Types.h"
#include "NodeLibrary.h"
#include "MetaPort.h"
#include "core/Editable.h"

#include <string>
#include <type_traits>
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
	class Transfer;

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

	class DAGBASE_API Port : public dagbase::Class, public dagbase::Editable
    {
    public:
        typedef std::vector<Port*> PortArray;

        enum PortFlags : std::uint32_t
        {
            FLAGS_NONE          = 0,
            OWN_META_PORT_BIT   = (1<<0),
            OWN_INPUTS_BIT      = (1<<1),
            OWN_OUTPUTS_BIT     = (1<<2),
            REMOVED_BIT         = (1<<3)
        };
    public:
	    Port() = default;

        explicit Port(PortID id, Node* parent, std::string name, PortType::Type type, PortDirection::Direction dir, PortFlags flags=FLAGS_NONE);

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

        [[nodiscard]] const PortArray& outgoingConnections() const
        {
            return _outgoingConnections;
        }

        [[nodiscard]] size_t numOutgoingConnections() const
        {
            return _outgoingConnections.size();
        }

        [[nodiscard]] const PortArray& incomingConnections() const
        {
            return _incomingConnections;
        }

        [[nodiscard]] size_t numIncomingConnections() const
        {
            return _incomingConnections.size() -
                   std::count_if(_incomingConnections.begin(), _incomingConnections.end(), [](Port *p) {
                       return p->isMarkedRemoved();
                   });
        }

        void unmarkAllConnections()
        {
            for (auto p : _incomingConnections)
            {
                p->unmarkRemoved();
            }

            for (auto p : _outgoingConnections)
            {
                p->unmarkRemoved();
            }
        }

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

        void markIncomingRemoved(Port *n)
        {
            for (auto p : _incomingConnections)
            {
                if (n==p)
                {
                    n->markRemoved();
                }
            }
        }

        void markOutgoingRemoved(Port* n)
        {
            for (auto p : _outgoingConnections)
            {
                if (n==p)
                {
                    n->markRemoved();
                }
            }
        }

        [[nodiscard]] bool hasNoDependencies() const
        {
            return  _incomingConnections.empty();
        }

		void addOutgoingConnection(Port* port)
		{
			if (port != nullptr)
			{
				_outgoingConnections.emplace_back(port);
			}
		}

		void removeOutgoingConnection(Port const * port)
		{
			if (auto const it=std::find(_outgoingConnections.begin(), _outgoingConnections.end(),port); it!=_outgoingConnections.end())
			{
				_outgoingConnections.erase(it);
			}
		}

		void addIncomingConnection(Port* port)
		{
			if (port!=nullptr)
			{
				_incomingConnections.emplace_back(port);
			}
		}

		void removeIncomingConnection(Port const* port)
		{
			if (auto const it = std::find(_incomingConnections.begin(), _incomingConnections.end(), port); it != _incomingConnections.end())
			{
				_incomingConnections.erase(it);
			}
		}

		[[nodiscard]]auto findOutgoingConnection(const Port& port)
		{
			return std::find(_outgoingConnections.begin(), _outgoingConnections.end(), &port);
		}

		auto findIncomingConnection(const Port& port)
		{
			return std::find(_incomingConnections.begin(), _incomingConnections.end(), &port);
		}

		[[nodiscard]]bool isCompatibleWith(const Port& other) const
		{
			return ((type() == other.type()));// || (type() == TYPE_INTEGER && other.type() == TYPE_DOUBLE) || (type() == TYPE_BOOL && other.type() == TYPE_INTEGER) || (type() == TYPE_BOOL && other.type() == TYPE_DOUBLE));
		}

		template<typename F>
		void eachOutgoingConnection(F f)
		{
			for (auto & _outgoingConnection : _outgoingConnections)
			{
				std::invoke(f, _outgoingConnection);
			}
		}

		template<typename F>
		void eachIncomingConnection(F f)
		{
			for (auto & _incomingConnection : _incomingConnections)
			{
				std::invoke(f, _incomingConnection);
			}
		}

        void setParent(Node* parent)
        {
            _parent = parent;
        }

        [[nodiscard]]Node* parent()
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

        virtual Transfer* connectTo(Port& dest) = 0;

        void reconnectTo(NodeSet const& selection, Node* newDest, KeyGenerator& keyGen);

        void reconnectFrom(NodeSet const& selection, Node* newSource, KeyGenerator& keyGen);

        void disconnect(Port& dest)
        {
            removeOutgoingConnection(&dest);
            dest.removeIncomingConnection(this);
        }

        virtual Transfer* setDestination(Transfer* transfer) = 0;

        virtual void accept(ValueVisitor& visitor) const = 0;

        virtual void accept(SetValueVisitor& visitor) = 0;

        bool isConnectedTo(Port* other) const
        {
            return (std::find(_incomingConnections.begin(), _incomingConnections.end(), other) != _incomingConnections.end() ||
                   std::find(_outgoingConnections.begin(), _outgoingConnections.end(), other) != _outgoingConnections.end());
        }

        virtual Port* clone(CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen) const = 0;

        dagbase::OutputStream& writeToStream(dagbase::OutputStream& str, NodeLibrary& nodeLib, Lua& lua) const override;

        [[nodiscard]]virtual bool equals(const Port& other) const
        {
            return false;
        }

        virtual void debug(dagbase::DebugPrinter& printer) const;

        virtual DebugPrinter &toLua(DebugPrinter &printer);

        [[nodiscard]]const char* className() const override
        {
            return "Port";
        }

        virtual Variant find(std::string_view path) const;

	    static std::string portFlagsToString(PortFlags flags);

	    static PortFlags parsePortFlags(const std::string& str);
    protected:
		PortArray _outgoingConnections;
		PortArray _incomingConnections;

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
	};
}
