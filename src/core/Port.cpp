#include "config/config.h"

#include "core/Port.h"
#include "core/TypedPort.h"
#include "core/Node.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "core/NodeLibrary.h"
#include "util/DebugPrinter.h"
#include "core/CloningFacility.h"
#include "core/KeyGenerator.h"
#include "util/Searchable.h"
#include "util/enums.h"

namespace dagbase
{
    Port::Port(PortID id, Node *parent, std::string name, PortType::Type type, PortDirection::Direction dir, PortFlags flags)
        :
        _name(std::move(name)),
        _type(type),
        _direction(dir),
        _id(id),
        _parent(parent),
        _flags(flags)
    {
        // Do nothing.
    }

    Port::~Port()
    {
        eachOutgoingConnection([this](Port *port)
                               {
                                   port->removeIncomingConnection(this);
                               });
        eachIncomingConnection([this](Port *port)
                               {
                                   port->removeOutgoingConnection(this);
                               });

//        if (_flags & OWN_INPUTS_BIT)
//        {
//            for (auto input : _incomingConnections)
//            {
//                delete input;
//            }
//        }
//
//        if (_flags & OWN_OUTPUTS_BIT)
//        {
//            for (auto output : _outgoingConnections)
//            {
//                delete output;
//            }
//        }
    }

    Port::Port(const Port &other, CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen)
        :
        _name(other._name),
        _type(other._type),
        _direction(other._direction),
        _id(other._id),
        _parent(other._parent),
        _flags(static_cast<PortFlags>(other._flags|OWN_META_PORT_BIT))
    {
        std::uint64_t otherId = 0;
        facility.putOrig(const_cast<Port*>(&other), &otherId);
        facility.addClone(otherId, this);
        if ((copyOp & CopyOp::GENERATE_UNIQUE_ID_BIT) != 0x0 && keyGen != nullptr)
        {
            _id = keyGen->nextPortID();
        }
        if (copyOp & CopyOp::DEEP_COPY_INPUTS_BIT)
        {
            setFlag(OWN_INPUTS_BIT);
            for (auto it=other._incomingConnections.begin(); it!=other._incomingConnections.end(); ++it)
            {
                std::uint64_t connectionId = 0;
                Port* connection = nullptr;

                if (facility.putOrig(*it, &connectionId))
                {
                    connection = (*it)->clone(facility, copyOp, keyGen);
                }
                else
                {
                    connection = static_cast<Port*>(facility.getClone(connectionId));
                }
                _incomingConnections.emplace_back(connection);
            }
        }

        if (copyOp & CopyOp::DEEP_COPY_OUTPUTS_BIT)
        {
            setFlag(OWN_OUTPUTS_BIT);
            for (auto it=other._outgoingConnections.begin(); it!=other._outgoingConnections.end(); ++it)
            {
                std::uint64_t connectionId = 0;
                Port* connection = nullptr;

                if (facility.putOrig(*it, &connectionId))
                {
                    connection = (*it)->clone(facility, copyOp, keyGen);
                }
                else
                {
                    connection = static_cast<Port*>(facility.getClone(connectionId));
                }
                _outgoingConnections.emplace_back(connection);
            }
        }

        if (copyOp & CopyOp::DEEP_COPY_PARENT_BIT)
        {
            std::uint64_t parentId = 0;
            if (facility.putOrig(other._parent,&parentId))
            {
                _parent = other._parent->clone(facility,copyOp,keyGen);
            }
            else
            {
                _parent = static_cast<Node*>(facility.getClone(parentId));
            }
        }
    }

    //! Reconnect to nodes of our output connections that are in the selection by adding new Ports
    //! on the new destination Node.
    void dagbase::Port::reconnectTo(NodeSet const &selection, Node *newDest, KeyGenerator& keyGen)
    {
        //   if the destination input port has a parent of oldDest then
        CloningFacility facility;

        for (auto &oldInput: _outgoingConnections)
        {
            if (auto it = selection.m.find(oldInput->parent()); it == selection.end())
            {
                // Create a new input port in newDest, without deep copying inputs and outputs.
                Port *newInput = oldInput->clone(facility, CopyOp{dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT}, &keyGen);
                // Connect the output port to the new input port
                // Disconnect the old input port
                newDest->addDynamicPort(newInput, MetaPort::FLAGS_OWN_BIT);
                newInput->_incomingConnections.emplace_back(this);
                // Create a new output from this, without deep copying inputs and outputs.
                Port *newOutput = this->clone(facility, CopyOp{dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT}, &keyGen);
                newDest->addDynamicPort(newOutput, MetaPort::FLAGS_OWN_BIT);
                newOutput->_outgoingConnections.emplace_back(oldInput);
                auto itOld = oldInput->findIncomingConnection(*this);
                if (itOld != oldInput->_incomingConnections.end())
                {
                    (*itOld) = newOutput;
                }
                oldInput = newInput;

            }
        }

    }

    //! Reconnect from nodes of our incoming connections that are in the selection by  adding Ports on the new source
    //! \note newSource is typically a Boundary node used during an AddChild operation.
    void dagbase::Port::reconnectFrom(NodeSet const &selection, Node *newSource, KeyGenerator& keyGen)
    {
        CloningFacility facility;
        for (auto &oldOutput: _incomingConnections)
        {
            if (auto it = selection.m.find(oldOutput->parent()); it == selection.end())
            {
                Port *newOutput = oldOutput->clone(facility, CopyOp{dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT}, &keyGen);
                newSource->addDynamicPort(newOutput, MetaPort::FLAGS_OWN_BIT);
                newOutput->_outgoingConnections.emplace_back(this);

                Port *newInput = this->clone(facility, CopyOp{ dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT }, &keyGen);
                newSource->addDynamicPort(newInput, MetaPort::FLAGS_OWN_BIT);
                newInput->_incomingConnections.emplace_back(oldOutput);
                if (auto itOld = oldOutput->findOutgoingConnection(*this); itOld != oldOutput->_outgoingConnections.end())
                {
                    (*itOld) = newInput;
                }
                oldOutput = newOutput;
            }
        }
    }

    dagbase::OutputStream &Port::writeToStream(dagbase::OutputStream &str, NodeLibrary& nodeLib, Lua& lua) const
    {
        str.writeHeader("Port");
        str.writeField("id");
        str.writeInt64(_id);
        str.writeField("name");
        str.writeString(_name, true);
        str.writeField("type");
        str.writeUInt32(_type);
        str.writeField("direction");
        str.writeUInt32(_direction);
        str.writeField("parent");
        if (str.writeRef(_parent))
        {
            std::string className = _parent->className();
            str.writeField("className");
            str.writeString(className, true);
            _parent->writeToStream(str, nodeLib, lua);
        }
        str.writeField("numOutgoingConnections");
        str.writeUInt32(_outgoingConnections.size());
        str.writeField("outgoingConnections");
        for (auto c: _outgoingConnections)
        {
            if (str.writeRef(c))
            {
                c->writeToStream(str, nodeLib, lua);
            }
        }

        str.writeField("numIncomingConnections");
        str.writeUInt32(_incomingConnections.size());
        str.writeField("incomingConnections");
        for (auto c : _incomingConnections)
        {
            if (str.writeRef(c))
            {
                c->writeToStream(str, nodeLib, lua);
            }
        }
        str.writeFooter();
        return str;
    }

    std::ostream& Port::toLua(std::ostream &str)
    {
        str << "id = " << _id << ", ";
        str << "name = " << _name << ", ";
        str << "type = " << PortType::toString(_type) << ", ";
        str << "direction = " << PortDirection::toString(_direction) << ", ";
        str << "class = \"" << className() << "\", ";
        if (_parent!=nullptr)
        {
            str << "parent = \"" << _parent->name() << "\", ";
        }
        else
        {
            str << "parent = nil, ";
        }

        return str;
    }

    void Port::debug(dagbase::DebugPrinter& printer) const
    {
        printer.printIndent().print(this);
        printer.println("");
        printer.println("id: " + std::to_string(_id));
        printer.println("name: " + _name);
        printer.println("type: " + std::string(PortType::toString(_type)));
        printer.println("direction: " + std::string(PortDirection::toString(_direction)));
        if (_parent!=nullptr)
        {
            printer.println("parent:" + _parent->name() + "(" + std::to_string(_parent->id()) + ")");
        }
        else
        {
            printer.println("parent:\"<none>\"");
        }
        printer.println("remove: " + std::to_string(isMarkedRemoved()));
        printer.println("value:");
        printer.println("outgoingConnections:");
        printer.println("{");
        printer.indent();
        for (auto connection : _outgoingConnections)
        {
            if (auto it = connection->findIncomingConnection(*this); it != connection->_incomingConnections.end())
            {
                printer.println(std::to_string(connection->id()));
            }
            else
            {
                printer.println(std::to_string(connection->id()) + " X");
            }
        }
        printer.outdent();
        printer.println("}");
        printer.println("incomingConnections");
        printer.println("{");
        printer.indent();
        for (auto connection : _incomingConnections)
        {
            if (auto it = connection->findOutgoingConnection(*this); it != connection->_outgoingConnections.end())
            {
                printer.println(std::to_string(connection->id()));
            }
            else
            {
                printer.println(std::to_string(connection->id()) + " X");
            }
        }
        printer.outdent();
        printer.println("}");
        //printer.print(_value);
    }

    Port::Port(dagbase::InputStream &str, NodeLibrary& nodeLib, dagbase::Lua& lua)
    {
        Port::readFromStream(str, nodeLib, lua);
    }

    dagbase::InputStream& Port::readFromStream(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& lua)
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);
        str.readField(&fieldName);
        str.addObj(this);
        std::int64_t id{0};
        str.readInt64(&id);
        _id = id;
        str.readField(&fieldName);
        str.readString(&_name, true);
        str.readField(&fieldName);
        std::uint32_t rawType{0};
        str.readUInt32(&rawType);
        _type = static_cast<PortType::Type>(rawType);
        str.readField(&fieldName);
        std::uint32_t rawDirection{0};
        str.readUInt32(&rawDirection);
        _direction = static_cast<PortDirection::Direction>(rawDirection);
        dagbase::Stream::ObjId metaPortId = 0;
        //        dagbase::Stream::ObjId parentId = 0;
        str.readField(&fieldName);
        _parent = str.readRef<Node>("Node", nodeLib, lua);
        std::uint32_t numOutgoingConnections = 0;
        str.readField(&fieldName);
        str.readUInt32(&numOutgoingConnections);
        str.readField(&fieldName);
        for (std::uint32_t i=0; i<numOutgoingConnections; ++i)
        {
            Port* port = str.readRef<Port>("Port",nodeLib, lua);
            if (port!=nullptr)
            {
                addOutgoingConnection(port);
            }
        }
        std::uint32_t numIncomingConnections = 0;
        str.readField(&fieldName);
        str.readUInt32(&numIncomingConnections);
        str.readField(&fieldName);
        for (std::uint32_t i=0; i<numIncomingConnections; ++i)
        {
            Port* port = str.readRef<Port>("Port", nodeLib, lua);
            if (port != nullptr)
            {
                addIncomingConnection(port);
            }

        }
        str.readFooter();
        return str;
    }

    bool Port::operator==(const Port &other) const
    {
        if (this == &other)
            return true;

        if (_name != other._name)
            return false;

        if (_type != other._type)
            return false;

        if (_direction != other._direction)
            return false;

        // if (_parent!=nullptr && other._parent!=nullptr && _parent->id() != other._parent->id())
        // {
        //     return false;
        // }

        if (_outgoingConnections.size() != other._outgoingConnections.size())
        {
            return false;
        }
        //
        // for (auto it=_outgoingConnections.begin(); it!=_outgoingConnections.end(); ++it)
        // {
        //     auto it2 = other._outgoingConnections.begin() + std::distance(_outgoingConnections.begin(), it);
        //
        //     if ((*it)->id() != (*it2)->id())
        //     {
        //         return false;
        //     }
        // }
        //
        if (_incomingConnections.size() != other._incomingConnections.size())
        {
            return false;
        }
        //
        // for (auto it=_incomingConnections.begin(); it!=_incomingConnections.end(); ++it)
        // {
        //     auto it2 = other._incomingConnections.begin() + std::distance(_incomingConnections.begin(), it);
        //
        //     if ((*it)->id() != (*it2)->id())
        //     {
        //         return false;
        //     }
        // }

        return true;
    }

    Variant Port::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "name", _name);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "type", std::uint32_t(_type));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "direction", std::uint32_t(_direction));
        if (retval.has_value())
            return retval;

        if (_parent)
        {
            retval = findEndpoint(path, "parentClass", _parent->className());
            if (retval.has_value())
                return retval;
        }
        retval = findEndpoint(path, "numIncomingConnections", std::uint32_t(numIncomingConnections()));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "numOutgoingConnections", std::uint32_t(numOutgoingConnections()));
        if (retval.has_value())
            return retval;

        return {};
    }

    std::string Port::portFlagsToString(PortFlags flags)
    {
        std::string retval;

        if (flags == PortFlags::FLAGS_NONE)
            return "FLAGS_NONE";

        BIT_NAME(flags, OWN_META_PORT_BIT, retval)
        BIT_NAME(flags, OWN_INPUTS_BIT, retval)
        BIT_NAME(flags, OWN_OUTPUTS_BIT, retval)
        BIT_NAME(flags, REMOVED_BIT, retval)

        if (!retval.empty() && retval.back() == ' ')
            retval.pop_back();

        return retval;
    }

    Port::PortFlags Port::parsePortFlags(const std::string& str)
    {
        PortFlags retval{FLAGS_NONE};

        TEST_BIT(OWN_META_PORT_BIT, str, retval )
        TEST_BIT(OWN_INPUTS_BIT, str, retval )
        TEST_BIT(OWN_OUTPUTS_BIT, str, retval )
        TEST_BIT(REMOVED_BIT, str, retval )

        return retval;
    }

    /*
    TypedPortBase::TypedPortBase(Node* parent, MetaPort* metaPort)
		:
		Port(parent, metaPort)
	{
		// Do nothing.
	}
*/
}
