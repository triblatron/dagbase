#include "config/config.h"

#include "core/Node.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "util/DebugPrinter.h"
#include "core/NodeLibrary.h"
#include "core/KeyGenerator.h"
#include "core/CloningFacility.h"
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
        std::uint64_t otherId = 0;
        bool shouldClone = facility.putOrig(const_cast<Node*>(&other), &otherId);
        facility.addClone(otherId, this);
        if ((copyOp & CopyOp::GENERATE_UNIQUE_ID_BIT)!=0x0 && keyGen!=nullptr)
        {
            _id = keyGen->nextNodeID();
        }
        _pos[0] = other._pos[0];
        _pos[1] = other._pos[1];
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
        str.readFooter();
        return str;
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
        // if (_name != other._name)
        // {
        //     return false;
        // }

        if (_category != other._category)
        {
            return false;
        }

        if (_flags != other._flags)
        {
            return false;
        }

        return true;
    }

    void Node::debug(dagbase::DebugPrinter& printer) const
    {
        printer.printIndent().print(this);
        printer.println("");
        printer.println("id: " + std::to_string(_id));
        printer.println("name: " + _name);
        printer.println("category: " + std::string(NodeCategory::toString(_category)));
        printer.println("flags: " + std::string(Node::flagsToString(_flags)));
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

        return {};
    }

    std::ostream &Node::toLua(std::ostream &str)
    {
        str << "{ ";
        str << "id = " << _id << ", ";
        str << "class = \"" << className() << "\", ";
        str << "name = \"" << _name << "\", ";
        str << "category = \"" << NodeCategory::toString(_category) << "\", ";
        str << "flags = " << _flags << ", ";
        str << "ports = { ";
        for (size_t i=0; i<totalPorts(); ++i)
        {
            str << "{ ";
            dynamicPort(i)->toLua(str);
            str << " }, ";
        }
        str << " }";
        str << " }";

        return str;
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
}
