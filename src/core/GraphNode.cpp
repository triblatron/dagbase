//
// Created by Tony Horrobin on 01/07/2026.
//

#include "config/config.h"

#include "core/GraphNode.h"
#include "io/InputStream.h"
#include "core/CloningFacility.h"
#include "core/Graph.h"

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
        std::uint64_t graphId{0};
        bool shouldCloneGraph = facility.putOrig(other._graph, &graphId);
        if (shouldCloneGraph)
        {
            _graph = other._graph->clone(facility, copyOp, keyGen);
        }
        else
        {
            _graph = static_cast<Graph*>(facility.getClone(graphId));
        }
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

            GraphNode::addDynamicPort(portClone, other._dynamicMetaPorts[i].flags);
        }
        // The other will not have a Graph if we are cloning from a NodeLibrary.
        if (other._graph && other._graph->parent())
            other._graph->parent()->addChild(_graph);
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
        for (std::size_t i=0; i<_dynamicPorts.size(); ++i)
        {
            if (_dynamicMetaPorts[i].isOwned())
            {
                delete _dynamicPorts.a[i];
            }
        }
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
        bool baseEqual = operator==(other);
        if (!baseEqual)
            return false;

        const auto& otherNode = dynamic_cast<const GraphNode&>(other);
        if (!_graph && otherNode._graph)
            return false;

        if (_graph && !otherNode._graph)
            return false;

        return *_graph == *otherNode._graph;
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

    void GraphNode::debug(dagbase::DebugPrinter &printer) const
    {
        Node::debug(printer);
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
        if (_graph)
        {
            printer.indent();
            _graph->debug(printer);
            printer.outdent();
        }
    }
}
