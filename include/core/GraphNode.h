//
// Created by Tony Horrobin on 01/07/2026.
//

#ifndef DAGUI_GRAPHNODE_H
#define DAGUI_GRAPHNODE_H

#include "config/DagBaseExport.h"

#include "core/Node.h"
#include "util/SearchableArray.h"

namespace dagbase
{
    class DAGBASE_API GraphNode : public Node
    {
    public:
        GraphNode(dagbase::KeyGenerator& keyGen, std::string name, dagbase::NodeCategory::Category category=dagbase::NodeCategory::CAT_NONE);

        GraphNode(const GraphNode& other, dagbase::CloningFacility& facility, dagbase::CopyOp copyOp, dagbase::KeyGenerator* keyGen);

        GraphNode(GraphNode&& other) noexcept = default;

        GraphNode(dagbase::InputStream& str, dagbase::NodeLibrary& nodeLib, dagbase::Lua& lua);

        ~GraphNode() override;

        GraphNode& operator=(const GraphNode& other);

        GraphNode& operator=(GraphNode&& other) = default;

        void setGraph(Graph* graph)
        {
            _graph = graph;
        }

        Graph* graph()
        {
            return _graph;
        }

        [[nodiscard]]const char* className() const override
        {
            return "GraphNode";
        }
        //! Compare for equality with another Node.
        //! \note Typically downcasts to a concrete type to determine a result.
        [[nodiscard]]bool equals(const Node& other) const override;

        [[nodiscard]]dagbase::MetaPort const* dynamicMetaPort(size_t index) const override
        {
            if (index<_dynamicMetaPorts.size())
            {
                return &_dynamicMetaPorts[index];
            }

            return nullptr;
        }

        void addDynamicPort(dagbase::Port* port, MetaPort::Flags flags) override
        {
            if (port != nullptr)
            {
                // We do not own the Port, so we are not its parent.
                port->setSharedParent(this);
                _dynamicPorts.a.emplace_back(port);
                MetaPort desc;
                desc.flags = flags;
                _dynamicMetaPorts.emplace_back(desc);
            }
        }

        dagbase::Port* dynamicPort(size_t index) override
        {
            if (index<_dynamicPorts.size())
            {
                return _dynamicPorts.a[index];
            }

            return nullptr;
        }

        const dagbase::Port* dynamicPort(size_t index) const override
        {
            if (index<_dynamicPorts.size())
            {
                return _dynamicPorts.a[index];
            }

            return nullptr;
        }

        std::size_t totalPorts() const override
        {
            return _dynamicPorts.size();
        }

        //! Create a Node of the same type as this from a stream.
        //! \param[in] str The stream from which to read the data required to create the Node.
        //! \param[in] nodeLib The NodeLibrary to create Ports
        //! \note Making this means we know the exact type of the node and do not have to resort to
        //! dynamic_cast<>() or similar.
        Node* create(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& Lua) override;

        //! Clone ourself to support the Prototype pattern
        //! \note A deep copy of Ports is required.
        //! \note Since this method is virtual, we know the exact type
        //! and can just call the copy constructor on *this.
        Node* clone(CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen) override
        {
            return new GraphNode(*this, facility, copyOp, keyGen);
        }

        dagbase::Variant find(std::string_view path) const override;
    private:
        typedef std::vector<dagbase::MetaPort> MetaPortArray;
        MetaPortArray _dynamicMetaPorts;
        typedef SearchableArray<std::vector<dagbase::Port*>> PortArray;
        PortArray _dynamicPorts;
        Graph* _graph{nullptr};
    };
}
#endif //DAGUI_GRAPHNODE_H
