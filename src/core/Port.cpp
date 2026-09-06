#include "config/config.h"

#include "core/Port.h"
#include "core/TypedPort.h"
#include "core/Node.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "core/NodeLibrary.h"
#include "util/DebugPrinter.h"
#include "core/CloningFacility.h"
#include "core/Graph.h"
#include "core/KeyGenerator.h"
#include "core/SignalPathTable.h"
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
        _sharedParent(other._sharedParent),
        _flags(static_cast<PortFlags>(other._flags))
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
        }

        if (copyOp & CopyOp::DEEP_COPY_OUTPUTS_BIT)
        {
            setFlag(OWN_OUTPUTS_BIT);
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


        SignalPathTable::FindResultFrom result;
        if (parent() && parent()->parent())
        {
            parent()->parent()->findBySource(id(), &result);
        }
        std::vector<SignalPath*> outgoingConnections(result.begin(), result.end());
        for (auto signalPath: outgoingConnections)
        {
            assert(signalPath->isRemoved());
            auto oldInput = signalPath->dest();
            if (auto it = selection.m.find(oldInput->parent()); !oldInput->isMarkedRemoved() && it == selection.end())
            {
                // Create a new input port in newDest, without deep copying inputs and outputs.
                Port *newInput = oldInput->clone(facility, CopyOp{dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT}, &keyGen);
                // Connect the output port to the new input port
                // Disconnect the old input port
                newDest->addDynamicPort(newInput, MetaPort::FLAGS_OWN_BIT);

                newInput->addIncomingConnection(this, newDest->parent(), keyGen);
                // Create a new output from this, without deep copying inputs and outputs.
                Port *newOutput = this->clone(facility, CopyOp{dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT}, &keyGen);
                newDest->addDynamicPort(newOutput, MetaPort::FLAGS_OWN_BIT);
                newOutput->addOutgoingConnection(oldInput, signalPath->parent(), keyGen);

                oldInput->replaceIncomingConnection(this, newOutput);
                // signalPath->parent()->reinsertSignalPath(signalPath, signalPath->source(), newInput);
            }
        }
    }

    //! Reconnect from nodes of our incoming connections that are in the selection by  adding Ports on the new source
    //! \note newSource is typically a Boundary node used during an AddChild operation.
    void dagbase::Port::reconnectFrom(NodeSet const &selection, Node *newSource, KeyGenerator& keyGen)
    {
        CloningFacility facility;
        SignalPathTable::FindResultFrom result;
        if (parent() && parent()->parent())
        {
            parent()->parent()->findByDest(id(), &result);
        }
        std::vector<SignalPath*> oldIncomingSignalPaths(result.begin(), result.end());
        for (auto signalPath: oldIncomingSignalPaths)
        {
            assert(signalPath->isRemoved());
            auto oldOutput = signalPath->source();
            if (auto it = selection.m.find(oldOutput->parent()); !oldOutput->isMarkedRemoved() && it == selection.end())
            {
                Port *newOutput = oldOutput->clone(facility, CopyOp{dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT}, &keyGen);
                newSource->addDynamicPort(newOutput, MetaPort::FLAGS_OWN_BIT);
                newOutput->addOutgoingConnection(this, newSource->parent(), keyGen);
                Port *newInput = this->clone(facility, CopyOp{ dagbase::CopyOp::GENERATE_UNIQUE_ID_BIT }, &keyGen);
                newSource->addDynamicPort(newInput, MetaPort::FLAGS_OWN_BIT);
                newInput->addIncomingConnection(oldOutput, signalPath->parent(), keyGen);
                oldOutput->replaceOutgoingConnection(this, newInput);
                // signalPath->parent()->reinsertSignalPath(signalPath, newOutput, signalPath->dest());
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
        str.writeField("sharedParent");
        if (str.writeRef(_sharedParent))
        {
            std::string className = _sharedParent->className();
            str.writeField("className");
            str.writeString(className, true);
            _sharedParent->writeToStream(str, nodeLib, lua);
        }
        str.writeFooter();
        return str;
    }

    DebugPrinter &Port::toLua(DebugPrinter &printer) const
    {
        printer.printIndent().print("id = ").print(_id).print(",\n");
        printer.printIndent().print("name = \"").print(_name).print("\",\n");
        printer.printIndent().print("type = \"").print(PortType::toString(_type)).print("\",\n");
        printer.printIndent().print("direction = \"").print(PortDirection::toString(_direction)).print("\",\n");
        printer.printIndent().print("class = \"").print(className()).print("\",\n");
        printer.printIndent().print("flags = \"").print(portFlagsToString(_flags)).print("\",\n");
        if (_parent!=nullptr)
        {
            printer.printIndent().print("parent = \"").print(_parent->name()).print("\",\n");
        }
        else
        {
            printer.println("parent = nil, ");
        }
        if (_sharedParent!=nullptr)
        {
            printer.printIndent().print("sharedParent = \"").print(_sharedParent->name()).print("\",\n");
        }
        else
        {
            printer.println("sharedParent = nil, ");
        }

        return printer;
    }

    void Port::debug(dagbase::DebugPrinter& printer) const
    {
        printer.printIndent().print(this);
        printer.println("");
        printer.println("id: " + std::to_string(_id));
        printer.println("class: " + std::string(className()));
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
        if (_sharedParent!=nullptr)
        {
            printer.println("sharedParent:" + _sharedParent->name() + "(" + std::to_string(_sharedParent->id()) + ")");
        }
        else
        {
            printer.println("sharedParent:\"<none>\"");
        }
        printer.println("remove: " + std::to_string(isMarkedRemoved()));
        printer.println("value:");
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
        str.readField(&fieldName);
        _parent = str.readRef<Node>("Node", nodeLib, lua);
        str.readField(&fieldName);
        _sharedParent = str.readRef<Node>("Node", nodeLib, lua);
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

        if (_flags != other._flags)
            return false;

        // if (_parent!=nullptr && other._parent!=nullptr && _parent->id() != other._parent->id())
        // {
        //     return false;
        // }

        return true;
    }

    size_t Port::numOutgoingConnections() const
    {
        std::size_t total = 0;

        if (parent() && parent()->parent())
        {
            SignalPathTable::FindResultFrom result;

            parent()->parent()->findBySource(id(), &result);

            total = result.size();
        }

        if (sharedParent() && sharedParent()->parent())
        {
            SignalPathTable::FindResultFrom result;

            sharedParent()->parent()->findBySource(id(), &result);

            total += result.size();
        }

        return total;
    }

    void Port::addOutgoingConnection(Port *input, Graph* parent, KeyGenerator& keyGen)
    {
        if (parent)
        {
            parent->addSignalPath(new SignalPath(parent, keyGen, this, input));
        }
    }

    void Port::replaceOutgoingConnection(Port *needle, Port *replacement)
    {
        if (parent() && parent()->parent())
        {
            SignalPathTable::FindResultFrom outgoingConnections;
            parent()->parent()->findBySource(this->id(), &outgoingConnections);
            for (auto it=outgoingConnections.p.first; it!=outgoingConnections.p.second; ++it)
            {
                if ((*it) && !(*it)->isRemoved() && (*it)->dest() == needle)
                {
                    (*it)->parent()->parent()->reinsertSignalPath((*it), (*it)->source(),replacement);
                }
            }
        }
    }

    size_t Port::numIncomingConnections() const
    {
        std::size_t total = 0;

        if (parent() && parent()->parent())
        {
            SignalPathTable::FindResultFrom result;

            parent()->parent()->findByDest(id(), &result);

            total = result.size();
        }

        if (sharedParent() && sharedParent()->parent())
        {
            SignalPathTable::FindResultFrom result;

            sharedParent()->parent()->findByDest(id(), &result);

            total += result.size();
        }
        return total;
    }

    void Port::addIncomingConnection(Port *output, Graph* parent, KeyGenerator &keyGen)
    {
        if (parent)
        {
            parent->addSignalPath(new SignalPath(parent, keyGen, output, this));
        }
    }

    void Port::replaceIncomingConnection(Port *needle, Port *replacement)
    {
        if (parent() && parent()->parent())
        {
            SignalPathTable::FindResultFrom incomingConnections;
            parent()->parent()->findByDest(id(), &incomingConnections);
            for (auto it=incomingConnections.p.first; it!=incomingConnections.p.second; ++it)
            {
                if ((*it) && !(*it)->isRemoved() && (*it)->source() == needle)
                {
                    (*it)->parent()->reinsertSignalPath(*it, replacement, (*it)->dest());// setSource(replacement);
                }
            }
        }
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
            retval = findEndpoint(path, "parentName", _parent->name());
            if (retval.has_value())
                return retval;

            retval = findEndpoint(path, "parentClass", _parent->className());
            if (retval.has_value())
                return retval;
        }

        if (_sharedParent)
        {
            retval = findEndpoint(path, "sharedParentClass", _sharedParent->className());
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
