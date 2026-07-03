//
// Created by Tony Horrobin on 01/07/2026.
//

#include "config/config.h"

#include "core/GraphNode.h"
#include "io/InputStream.h"
#include "core/CloningFacility.h"

namespace dagbase
{
    GraphNode::GraphNode(dagbase::KeyGenerator &keyGen, std::string name, dagbase::NodeCategory::Category category)
        :
    Node(keyGen, std::move(name), category)
    {
    }

    GraphNode::GraphNode(const GraphNode &other, dagbase::CloningFacility &facility, dagbase::CopyOp copyOp,
        dagbase::KeyGenerator *keyGen)
            :
    Node(other, facility, copyOp, keyGen)
    {
        for (std::size_t i=0; i<other.totalPorts(); ++i)
        {
            auto* p = other._dynamicPorts.a[i];
            auto portClone = p->clone(facility, copyOp, keyGen);

            GraphNode::addDynamicPort(portClone, other._dynamicMetaPorts[i].flags);
        }
    }

    GraphNode::GraphNode(dagbase::InputStream &str, dagbase::NodeLibrary &nodeLib, dagbase::Lua &lua)
        :
    Node(str, nodeLib, lua)
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);
        Node::readFromStream(str, nodeLib, lua);
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
            _dynamicPorts.a[i] = nodeLib.instantiatePort(str, lua);
        }
        str.readFooter();
    }

    GraphNode::~GraphNode()
    {
        // Do not delete our shared Ports.
    }

    GraphNode & GraphNode::operator=(const GraphNode &other)
    {
        if (this != &other)
        {
            dagbase::CloningFacility facility;
            Node::operator=(other);

            for (std::size_t i=0; i<other.totalPorts(); ++i)
            {
                auto* p = other._dynamicPorts.a[i];
                addDynamicPort(p->clone(facility, dagbase::CopyOp{0}, nullptr), other._dynamicMetaPorts[i].flags);
            }
        }

        return *this;
    }

    bool GraphNode::equals(const Node &other) const
    {
        return operator==(other);
    }

    Node * GraphNode::create(dagbase::InputStream &str, NodeLibrary &nodeLib, dagbase::Lua &Lua)
    {
        return new GraphNode(str, nodeLib, Lua);
    }

    dagbase::Variant GraphNode::find(std::string_view path) const
    {
        Variant retval = Node::find(path);

        if (retval.has_value())
            return retval;

        retval = findInternal(path, "dynamicPort", _dynamicPorts);
        if (retval.has_value())
            return retval;

        return {};
    }
}
