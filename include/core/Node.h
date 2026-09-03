#pragma once

#include "config/DagBaseExport.h"

#include "NodeCategory.h"
#include "Port.h"
#include "MetaPort.h"
#include "Variant.h"
#include "util/SearchableArray.h"
#include <string>
#include <stdexcept>

#include "util/SearchableSet.h"

struct ImGuiContext;

namespace dagbase
{
    class InputStream;
    class OutputStream;
    class CloningFacility;
    class DebugPrinter;
    class KeyGenerator;
	struct NodeDescriptor;
    class NodeLibrary;

	class DAGBASE_API Node : public dagbase::Class, public Editable
	{
	public:
	    enum NodeFlags : std::uint32_t
		{
            NODE_NONE           = 0,
			NODE_INPUT_BIT		= 1U<<0U,
			NODE_OUTPUT_BIT		= 1U<<1U,
			NODE_INTERNAL_BIT	= 1U<<2U
		};
	public:
		Node() = default;

		explicit Node(KeyGenerator& keyGen, std::string name, NodeCategory::Category category=NodeCategory::CAT_NONE);

        explicit Node(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& lua);

		Node(const Node& other, CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen);

		Node(Node&&) = default;

        ~Node() override;

		Node& operator=(const Node&);

		Node& operator=(Node&&) = default;

        void edit(ImGuiContext* context) override;

		dagbase::InputStream& readFromStream(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& lua) override;

        //! Compare for equality with another Node.
        //! \note Typically downcasts to a concrete type to determine a result.
        [[nodiscard]]virtual bool equals(const Node& other, ComparisonFlags flags) const;

	    [[nodiscard]]const dagbase::MetaPort * dynamicMetaPort(size_t index) const
	    {
	        return &_dynamicMetaPorts[index];
	    }

	    [[nodiscard]]dagbase::MetaPort * dynamicMetaPort(size_t index)
	    {
	        return &_dynamicMetaPorts[index];
	    }

	    //! Add a non-null dynamic port
	    //! This is in addition to the intrinsic ports described by MetaPorts.
	    //! \note The default implementation throws an exception
	    void addDynamicPort(dagbase::Port* port, dagbase::MetaPort::Flags flags)
	    {
	        if (port != nullptr)
	        {
	            _dynamicPorts.a.emplace_back(port);
	            if ((flags & MetaPort::FLAGS_OWN_BIT)==MetaPort::FLAGS_OWN_BIT)
	            {
	                port->setParent(this);
	            }
	            else
	            {
	                port->setSharedParent(this);
	            }
	            MetaPort desc;
	            desc.flags = flags;
	            _dynamicMetaPorts.emplace_back(desc);
	        }
	    }

	    //! \return A Port corresponding to a given index
	    //! \note The index includes both built-in and dynamically added Ports.
	    //! \param[in] index The index of the Port, zero-based.
	    [[nodiscard]]dagbase::Port* dynamicPort(size_t index)
	    {
	        if (index < _dynamicPorts.size())
	        {
	            return _dynamicPorts.a[index];
	        }

	        return nullptr;
	    }

	    const dagbase::Port* dynamicPort(size_t index) const
	    {
	        if (index<_dynamicPorts.size())
	        {
	            return _dynamicPorts.a[index];
	        }

	        return nullptr;
	    }

        //! Create a Node of the same type as this from a stream.
        //! \param[in] str The stream from which to read the data required to create the Node.
        //! \param[in] nodeLib The NodeLibrary to create Ports
        //! \note Making this virtual means we know the exact type of the node and do not have to resort to
        //! dynamic_cast<>() or similar.
        virtual Node* create(dagbase::InputStream& str, NodeLibrary& nodeLib, dagbase::Lua& Lua) = 0;

	    //! Clone ourself to support the Prototype pattern
	    //! \note A deep copy of Ports is required.
	    //! \note Since this method is virtual, we know the exact type
	    //! and can just call the copy constructor on *this.
	    virtual Node* clone(CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen) = 0;

        //! Write ourself to a binary output stream
        //! \param[in] str The stream
	    //! \param[in] nodeLib : NodeLibrary The library from which the node came
	    //! \param[in] lua : Lua The Lua interpreter in case there are Ports which have Lua functions as values
        //! \param lua
        dagbase::OutputStream& writeToStream(dagbase::OutputStream& str, NodeLibrary& nodeLib, Lua &lua) const override;

        //! \return The total number of Ports in this Node, including intrinsic/static and dynamic/extrinsic Ports.
	    [[nodiscard]]size_t totalPorts() const
	    {
	        return _dynamicMetaPorts.size();
	    }

        //! Perform our computation based on inputs and settings.
        //! \note This has an empty default implementation.
        virtual void update()
        {
            // Do nothing.
        }

        //! Base class implementation of operator==().
        //! \note Typically called by a concrete implementation of equals().
        bool operator==(const Node& other) const;

	    void setParent(Graph* graph)
	    {
	        _parent = graph;
	    }

	    Graph* parent()
	    {
	        return _parent;
	    }

	    const Graph* parent() const
	    {
	        return _parent;
	    }

        void setId(NodeID id)
        {
            _id = id;
        }

		[[nodiscard]] NodeID id() const
        {
            return _id;
        }

		void setName(const std::string& name)
		{
			_name = name;
		}

		[[nodiscard]]const std::string& name() const
		{
			return _name;
		}

	    void setCategory(NodeCategory::Category category)
        {
            _category = category;
        }

        [[nodiscard]]NodeCategory::Category category() const
		{
			return _category;
		}

	    void setFlags(NodeFlags flags)
        {
            _flags = flags;
        }

	    NodeFlags flags() const
        {
            return _flags;
        }

        void setPosition(float x, float y)
        {
            _pos[0] = x;
            _pos[1] = y;
        }

        [[nodiscard]]const float* position() const
        {
            return _pos;
        }

        //! Find the index of a given Port.
        //! \retval ~0ULL if the Port cannot be found.
        [[nodiscard]]size_t indexOfPort(const Port* port) const
        {
            for (std::size_t i=0; i<totalPorts(); ++i)
            {
                if (dynamicPort(i)==port)
                {
                    return i;
                }
            }
            return size_t{~0ULL};
        }

        void reconnectInputs(NodeSet const& selection, Node* newSource, KeyGenerator& keyGen);

        void reconnectOutputs(NodeSet const& NodeSet, Node* newDest, KeyGenerator& keyGen);
        
        [[nodiscard]]bool hasNoDependencies();

        [[nodiscard]]size_t numIncomingConnections()
        {
            size_t total = 0;
            for (size_t portIndex=0; portIndex<totalPorts(); ++portIndex)
            {
                total += dynamicPort(portIndex)->numIncomingConnections();
            }

            return total;
        }

        [[nodiscard]]bool hasInputs()
        {
            for (std::size_t index=0; index<totalPorts(); ++index)
            {
                auto p = dynamicPort(index);

                if (p->dir() == PortDirection::DIR_IN)
                {
                    return true;
                }
            }

            return false;
        }

        [[nodiscard]]bool hasOutputs()
        {
            for (std::size_t index=0; index<totalPorts(); ++index)
            {
                auto p = dynamicPort(index);

                if (p->dir() == PortDirection::DIR_OUT)
                {
                    return true;
                }
            }

            return false;
        }

        //! Pretty-print this node for debugging purposes.
        virtual void debug(dagbase::DebugPrinter& printer) const;

	    virtual Variant find(std::string_view path) const;

        //! Convert this Node to a Lua representation.
        virtual DebugPrinter &toLua(DebugPrinter &printer) const;

        static std::string flagsToString(NodeFlags value);

        static NodeFlags parseFlags(const std::string& str);
	protected:
		typedef std::vector<dagbase::MetaPort> MetaPortArray;
		typedef SearchableArray<std::vector<dagbase::Port*>> PortArray;
		static void writeDynamicPorts(OutputStream& str, NodeLibrary& nodeLib, Lua& lua, const PortArray& ports, const MetaPortArray& metaPorts);
	    static void readDynamicPorts(InputStream& str, NodeLibrary& nodeLib, Lua& lua, PortArray& ports, MetaPortArray& metaPorts);
	    void clonePorts(const Node& other, CloningFacility& facility, CopyOp copyOp, KeyGenerator* keyGen);
	    void deleteDynamicPorts();
	private:
        NodeID _id{NodeID::INVALID_ID};
        std::string _name;
        // Position to allow for manual layout
        float _pos[2]{0,0};
		NodeCategory::Category _category{NodeCategory::CAT_UNKNOWN};
        NodeFlags _flags{ NODE_NONE };
	    MetaPortArray _dynamicMetaPorts;
	    PortArray _dynamicPorts;
	    Graph* _parent{nullptr};
	};

    struct CompareNodesById
    {
        bool operator()(const Node* a, const Node* b) const
        {
            return a->id() < b->id();
        }
    };
    typedef SearchableSet<VectorSet<Node*, CompareNodesById>> NodeSet;
}
