#include "config/config.h"

#include "core/Node.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "util/DebugPrinter.h"
#include "core/NodeLibrary.h"
#include "core/KeyGenerator.h"
#include "core/CloningFacility.h"
#include "core/Graph.h"
#include "util/enums.h"

#include "imgui.h"

#include <iostream>

#include "util/Searchable.h"

namespace dagbase
{
    Node::Node(KeyGenerator& keyGen, std::string name, NodeCategory::Category category)
    :
    _id(keyGen.nextNodeID()),
    _name(std::move(name)),
    _category(category)
    {
        // Do nothing.
    }

    Node::Node(const Node& other, CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen)
    :
    _id(other._id),
    _name(other._name),
    _category(other._category),
    _flags(other._flags)
    {
        //std::uint64_t otherId = 0;
        //bool shouldClone = facility.putOrig(const_cast<Node*>(&other), &otherId);
        //facility.addClone(otherId, this);
        if ((copyOp & CopyOp::GENERATE_UNIQUE_ID_BIT)!=0x0 && keyGen!=nullptr)
        {
            _id = keyGen->nextNodeID();
        }
        _pos[0] = other._pos[0];
        _pos[1] = other._pos[1];
    }

    Node::~Node()
    {
    }

    Node & Node::operator=(const Node & other)
    {
        if (this!=&other)
        {
            for (std::size_t i=0; i<other.totalPorts(); ++i)
            {
                CloningFacility facility;
                auto* p = other._dynamicPorts.a[i];
                addDynamicPort(p->clone(facility, dagbase::CopyOp{0}, nullptr), other._dynamicMetaPorts[i].flags);
            }
        }

        return *this;
    }

    void Node::edit(ImGuiContext* context)
    {
        ImGui::SetCurrentContext(context);

        for (std::size_t i = 0; i < totalPorts(); ++i)
        {
            auto port = dynamicPort(i);

            port->edit(context);
        }
    }

    //! Reconnect from newSource to each input Port whose parent is in selection by adding new Ports.
    void dagbase::Node::reconnectInputs(NodeSet const& selection, Node *newSource, KeyGenerator& keyGen)
    {
        // for each output port do
        for (std::size_t portIndex = 0; portIndex < totalPorts(); ++portIndex)
        {
            Port* p = dynamicPort(portIndex);
            if (p->dir() == PortDirection::DIR_IN)
            {
                p->reconnectFrom(selection, newSource, keyGen);
            }

        }
    }

    //! Reconnect from each output Port whose parent is in selection to newDest by adding new Ports.
    void dagbase::Node::reconnectOutputs(NodeSet const& selection, Node *newDest, KeyGenerator& keyGen)
    {
        // for each output port do
        for (std::size_t portIndex=0; portIndex<totalPorts(); ++portIndex)
        {
            Port* p = dynamicPort(portIndex);
            if (p->dir() == PortDirection::DIR_OUT)
            {
                p->reconnectTo(selection, newDest, keyGen);
            }

        }

    }

    bool Node::hasNoDependencies()
    {
        bool noDeps = true;
        for (std::size_t i=0; i<totalPorts(); ++i)
        {
            SignalPathTable::FindResultFrom result;
            parent()->findByDest(dynamicPort(i)->id(), &result);
            if (!result.empty())
            {
                noDeps = false;
            }
        }

        return noDeps;
    }

    dagbase::OutputStream &Node::writeToStream(dagbase::OutputStream &str, NodeLibrary& nodeLib, Lua &lua) const
    {
        str.writeHeader("Node");
        Class::writeToStream(str, nodeLib, lua);
        str.writeField("id");
        str.writeInt64(_id);
        str.writeField("name");
        str.writeString(_name, true);
        str.writeField("category");
        str.writeUInt32(_category);
        str.writeField("flags");
        str.writeUInt32(_flags);
        str.writeField("x");
        str.writeFloat(_pos[0]);
        str.writeField("y");
        str.writeFloat(_pos[1]);
        writeDynamicPorts(str, nodeLib, lua, _dynamicPorts, _dynamicMetaPorts);
        str.writeFooter();

        return str;
    }

    Node::Node(dagbase::InputStream &str, NodeLibrary& nodeLib, Lua &lua)
    {
        Node::readFromStream(str, nodeLib, lua);
    }

    dagbase::InputStream& Node::readFromStream(dagbase::InputStream& str, dagbase::NodeLibrary& nodeLib, dagbase::Lua& lua)
    {
        std::string className;
        str.readHeader(&className);
        str.addObj(this);
        Class::readFromStream(str, nodeLib, lua);
        std::string fieldName;
        str.readField(&fieldName);
        std::int64_t id{0};
        str.readInt64(&id);
        _id = id;
        str.readField(&fieldName);
        str.readString(&_name, true);
        str.readField(&fieldName);
        std::uint32_t category{0};
        str.readUInt32(&category);
        _category = static_cast<NodeCategory::Category>(category);
        str.readField(&fieldName);
        std::uint32_t flags{0};
        str.readUInt32(&flags);
        _flags = static_cast<Node::NodeFlags>(flags);
        str.readField(&fieldName);
        str.readFloat(&_pos[0]);
        str.readField(&fieldName);
        str.readFloat(&_pos[1]);
        readDynamicPorts(str, nodeLib, lua, _dynamicPorts, _dynamicMetaPorts);
        str.readFooter();
        return str;
    }

    bool Node::equals(const Node &other, ComparisonFlags flags) const
    {
        if ((flags & CMP_IDENT_BIT)!=0)
        {
            if (_id != other._id)
                return false;
        }

        if ((flags & CMP_NAME_BIT)!=0)
        {
            if (_name != other._name)
                return false;
        }

        if (_category != other._category)
        {
            return false;
        }

        if (_flags != other._flags)
        {
            return false;
        }

        if (_pos[0] != other._pos[0])
            return false;

        if (_pos[1] != other._pos[1])
            return false;

        return true;
    }

    bool Node::operator==(const Node &other) const
    {
        if (this == &other)
            return true;
        // if (_id != other._id)
        // {
        //     return false;
        // }
        //
        // We cannot compare names for templates and their instantiations
        // if (_name != other._name)
        // {
        //     return false;
        // }

        return equals(other, static_cast<ComparisonFlags>(CMP_IDENT_BIT|CMP_NAME_BIT|CMP_CONNECTIONS_BIT));
    }

    void Node::debug(dagbase::DebugPrinter& printer) const
    {
        printer.printIndent().print(this);
        printer.println("");
        printer.println("id: " + std::to_string(_id));
        printer.println("class: " + std::string(className()));
        printer.println("name: " + _name);
        printer.println("category: " + std::string(NodeCategory::toString(_category)));
        printer.println("flags: " + std::string(Node::flagsToString(_flags)));
        printer.indent();
        for (const auto port : _dynamicPorts)
        {
            port->debug(printer);
        }
        printer.outdent();
        printer.indent();
        for (const auto& metaPort : _dynamicMetaPorts)
        {
            metaPort.debug(printer);
        }
        printer.outdent();
    }

    Variant Node::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "id", id());
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "class", std::string(className()));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "totalPorts", static_cast<std::uint32_t>(totalPorts()));
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "dynamicPort", _dynamicPorts);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "x", _pos[0]);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "y", _pos[1]);
        if (retval.has_value())
            return retval;

        return {};
    }

    DebugPrinter &Node::toLua(DebugPrinter &printer) const
    {
        printer.printIndent().print("id = ").print(_id).print(",\n");
        printer.printIndent().print("class = \"").print(className()).print("\",\n");
        printer.printIndent().print("name = \"").print(_name).print("\",\n");
        printer.printIndent().print("category = \"").print(NodeCategory::toString(_category)).print("\",\n");
        printer.printIndent().print("flags = \"").print(flagsToString(_flags)).print("\", \n");
        printer.printIndent().print("position = {").print(_pos[0]).print(", ").print(_pos[1]).print("},\n");
        printer.println("ports =");
        printer.println("{");
        printer.indent();
        for (size_t i=0; i<totalPorts(); ++i)
        {
            printer.printIndent().print("[").print(i+1).print("] = \n");
            printer.println("{ ");
            printer.indent();
            dynamicPort(i)->toLua(printer);
            printer.outdent();
            printer.println("},");
        }
        printer.outdent();
        printer.println("},");
        printer.println("metaPorts=");
        printer.println("{");
        printer.indent();
        for (size_t i=0; i<totalPorts(); ++i)
        {
            printer.printIndent().print("[").print(i+1).print("] = \n");
            printer.println("{ ");
            printer.indent();
            dynamicMetaPort(i)->toLua(printer);
            printer.outdent();
            printer.println("},");
        }
        printer.outdent();
        printer.println("},");

        return printer;
    }

    std::string Node::flagsToString(NodeFlags value)
    {
        std::string retval;

        if (value == NODE_NONE)
            return "NODE_NONE";

        BIT_NAME(value, NODE_INPUT_BIT, retval)
        BIT_NAME(value, NODE_OUTPUT_BIT, retval)
        BIT_NAME(value, NODE_INTERNAL_BIT, retval)

        if (!retval.empty() && retval.back() == ' ')
            retval.pop_back();

        return retval;
    }

    Node::NodeFlags Node::parseFlags(const std::string& str)
    {
        NodeFlags value{ NODE_NONE };

        TEST_BIT(NODE_INPUT_BIT, str, value)
        TEST_BIT(NODE_OUTPUT_BIT, str, value)
        TEST_BIT(NODE_INTERNAL_BIT, str, value)

        return value;
    }

    void Node::writeDynamicPorts(OutputStream &str, NodeLibrary &nodeLib, Lua &lua, const PortArray &_dynamicPorts, const MetaPortArray &_dynamicMetaPorts)
    {
        str.writeField("numDynamicMetaPorts");
        str.writeUInt32(_dynamicMetaPorts.size());
        str.writeField("dynamicMetaPorts");
        for (auto const & p : _dynamicMetaPorts)
        {
            p.write(str);
        }
        str.writeField("numDynamicPorts");
        str.writeUInt32(_dynamicPorts.size());
        str.writeField("dynamicPorts");
        for (auto p : _dynamicPorts)
        {
            if (str.writeRef(p))
                p->writeToStream(str, nodeLib, lua);
        }
    }

    void Node::readDynamicPorts(InputStream &str, NodeLibrary& nodeLib, Lua& lua, PortArray &_dynamicPorts, MetaPortArray &_dynamicMetaPorts)
    {
        std::string fieldName;
        std::uint32_t numDynamicMetaPorts = 0;
        str.readField(&fieldName);
        str.readUInt32(&numDynamicMetaPorts);
        _dynamicMetaPorts.resize(numDynamicMetaPorts);
        str.readField(&fieldName);
        for (std::size_t i=0; i<numDynamicMetaPorts; ++i)
        {
            _dynamicMetaPorts[i].read(str);
        }
        std::uint32_t numDynamicPorts = 0;
        str.readField(&fieldName);
        str.readUInt32(&numDynamicPorts);
        _dynamicPorts.a.resize(numDynamicPorts);
        str.readField(&fieldName);
        for (std::size_t i=0; i<numDynamicPorts; ++i)
        {
            dagbase::Stream::ObjId portId{~0U};
            auto portRef = str.readRef(&portId);
            if (portId != 0)
            {
                if (portRef != nullptr)
                {
                    _dynamicPorts.a[i] = static_cast<dagbase::Port*>(portRef);
                }
                else
                {
                    _dynamicPorts.a[i] = nodeLib.instantiatePort(str, lua);
                }
            }
        }
    }

    void Node::clonePorts(const Node &other, CloningFacility &facility, CopyOp copyOp, KeyGenerator *keyGen)
    {
        for (std::size_t i=0; i<other.totalPorts(); ++i)
        {
            auto* p = other._dynamicPorts.a[i];
            std::uint64_t portId{0};
            auto shouldClone = facility.putOrig(p, &portId);
            Port* portClone = nullptr;
            if (shouldClone)
            {
                portClone = p->clone(facility, copyOp, keyGen);
            }
            else
            {
                portClone = static_cast<Port*>(facility.getClone(portId));
            }

            addDynamicPort(portClone, other._dynamicMetaPorts[i].flags);
        }
    }

    void Node::deleteDynamicPorts()
    {
        for (std::size_t i=0; i<_dynamicPorts.size(); ++i)
        {
            if (_dynamicPorts.a[i]->sharedParent() == this)
            {
                _dynamicPorts.a[i]->setSharedParent(nullptr);
            }
            if (_dynamicMetaPorts[i].isOwned())
            {
                delete _dynamicPorts.a[i];
            }
        }
    }
}
