//
// Created by Tony Horrobin on 01/07/2026.
//

#include "config/config.h"

#include "core/GraphNode.h"
#include "io/InputStream.h"
#include "core/CloningFacility.h"
#include "core/Graph.h"
#include "io/OutputStream.h"

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
            facility.addClone(graphId, _graph);
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
        // if ((copyOp & ADD_CHILD_GRAPHS_BIT)!=0 && other._graph && other._graph->parent())
        //     other._graph->parent()->addChild(_graph);
    }

    GraphNode::GraphNode(dagbase::InputStream &str, dagbase::NodeLibrary &nodeLib, dagbase::Lua &lua)
        :
    Node()
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);
        Node::readFromStream(str, nodeLib, lua);
        readDynamicPorts(str, nodeLib, lua, _dynamicPorts, _dynamicMetaPorts);
        str.readField(&fieldName);
        dagbase::Stream::ObjId id = 0;
        Graph* child = nullptr;
        auto ref = str.readRef(&id);
        if (id != 0)
        {
            if (ref != nullptr)
            {
                _graph = static_cast<Graph*>(ref);
            }
            else
            {
                _graph = new Graph(str, nodeLib, lua);
            }
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

    bool GraphNode::equals(const Node &other, ComparisonFlags flags) const
    {
        bool baseEqual = Node::equals(other, flags);
        if (!baseEqual)
            return false;

        const auto& otherNode = dynamic_cast<const GraphNode&>(other);
        if (!_graph && otherNode._graph)
            return false;

        if (_graph && !otherNode._graph)
            return false;

        return _graph->equals(*otherNode._graph, flags);
    }

    Node * GraphNode::create(dagbase::InputStream &str, NodeLibrary &nodeLib, dagbase::Lua &Lua)
    {
        return new GraphNode(str, nodeLib, Lua);
    }

    dagbase::OutputStream & GraphNode::writeToStream(dagbase::OutputStream &str, NodeLibrary &nodeLib, Lua &lua) const
    {
        str.writeHeader("GraphNode");
        Node::writeToStream(str, nodeLib, lua);
        writeDynamicPorts(str, nodeLib, lua, _dynamicPorts, _dynamicMetaPorts);
        str.writeField("graph");
        if (str.writeRef(_graph))
        {
            _graph->write(str, nodeLib, lua);
        }
        str.writeFooter();
        return str;
    }

    dagbase::Variant GraphNode::find(std::string_view path) const
    {
        Variant retval = Node::find(path);

        if (retval.has_value())
            return retval;

        retval = findInternal(path, "dynamicPort", _dynamicPorts);
        if (retval.has_value())
            return retval;

        if (_graph)
        {
            retval = findInternal(path, "graph", _graph);
            if (retval.has_value())
                return retval;
        }
        return {};
    }

    DebugPrinter & GraphNode::toLua(DebugPrinter &printer) const
    {
        Node::toLua(printer);
        printer.println("graph=");
        printer.println("{");
        printer.indent();
        if (_graph)
        {
            _graph->toLuaHelper(printer);
        }
        printer.outdent();
        printer.println("}");
        return printer;
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
