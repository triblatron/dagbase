#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"
#include "core/KeyGenerator.h"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <iterator>

namespace dagbase
{
    class ByteBuffer;
    class CloningFacility;
    class InputStream;
	class Lua;
    class OutputStream;
	class Table;
    class Node;
    class NodeLibrary;
    class Port;
	class SignalPath;
    class SelectionInterface;
    typedef std::vector<dagbase::Node*> NodeArray;

    //! A collection of Nodes, SignalPaths and child Graphs.
	class DAGBASE_API Graph : public KeyGenerator
	{
	public:
		Graph() = default;

        Graph(dagbase::InputStream& str, dagbase::NodeLibrary& nodeLib, dagbase::Lua& lua);

		Graph(const Graph& other, dagbase::CloningFacility& facility, dagbase::CopyOp copyOp, KeyGenerator* keyGen);

		Graph(Graph&&) = default;

		~Graph() override;

		Graph& operator=(const Graph&) = default;

		Graph& operator=(Graph&&) = default;

        bool operator==(const Graph& other) const;

        Graph* clone(dagbase::CloningFacility& facility, dagbase::CopyOp copyOp, KeyGenerator* keyGen);

        [[nodiscard]]Graph* parent()
        {
            return _parent;
        }

		[[nodiscard]] size_t numNodes() const
		{
			return _nodes.size();
		}

        //! Add a node if it is non-null, does nothing otherwise.
		void addNode(dagbase::Node* node);

		dagbase::Node* lastAddedNode()
		{
			return _lastAddedNode;
		}

        //! Remove a non-null node.
        void removeNode(dagbase::Node* node);

        void setNodeLibrary(dagbase::NodeLibrary* nodeLib)
        {
            if (nodeLib!=nullptr)
            {
                _nodeLib = nodeLib;
            }
        }

        //! Create a node of the supplied class if it exists.
        //! \throw runtime_error if the class does not exist
        //! \note Does not add the Node to the Graph.
        dagbase::Node* createNode(std::string const& className, std::string const& name);

        //! \retval nullptr if a dagbase::Node with the specified ID does not exist.
		dagbase::Node* node(dagbase::NodeID id)
		{
			if (auto const it = _nodes.find(id); it!=_nodes.end())
			{
				return it->second;
			}
			return nullptr;
		}

        //! Operate on each Node of this Graph without descending into child Graphs.
        template<typename F>
        void eachNode(F f)
        {
            for (auto p : _nodes)
            {
                if (!std::invoke(f,p.second))
                {
                    return;
                }
            }
        }

        //! Add a non-null port.
        void addPort(dagbase::Port* port);

        //! \retval nullptr If a Port with the specified ID does not exist.
        dagbase::Port* port(dagbase::PortID id)
        {
            if (auto const it=_ports.find(id); it!=_ports.end())
            {
                return it->second;
            }

            return nullptr;
        }

		[[nodiscard]] size_t numSignalPaths() const
		{
			return _signalPaths.size();
		}

        //! Add a non-null SignalPath
		void addSignalPath(dagbase::SignalPath* signalPath);

        void removeSignalPath(dagbase::SignalPath* signalPath);

		[[nodiscard]] dagbase::SignalPath* signalPath(dagbase::SignalPathID id)
		{
			if (auto const it=_signalPaths.find(id); it!=_signalPaths.end())
			{
				return it->second;
			}

			return nullptr;
		}

        //! Create a Graph from a Lua string representation.
        //! \retval nullptr if there is a syntax error.
		static Graph* fromString(dagbase::NodeLibrary& nodeLib, const char* str);

        //! Create a Graph from a Lua representation in a file.
        //! \retval nullptr if the file could not be found or there is a syntax error.
        static Graph* fromFile(dagbase::NodeLibrary& nodeLib, const char* filename);

        std::ostream& toLua(std::ostream& str);

        enum TopoSortResult
        {
            //! The sort completed successfully.
            OK,
            //! One or more cycles were found, preventing a valid sort order.
            CYCLES_DETECTED
        };

        //! Perform a topological sort of this Graph.
        //! \return A valid order if successful, undefined otherwise.
        TopoSortResult topologicalSort(NodeArray* order);

        //! Evaluate the nodes in this Graph using the given order.
        void evaluate(const NodeArray& order);

        bool hasEdges() const;

        //! Find all Nodes in this Graph and its children.
        void findAllNodes(NodeArray* nodes);

        dagbase::Node* findNode(const std::string& path);

        void findAllSignalPaths(std::vector<dagbase::SignalPath*>* allSignalPaths);

        //! Add a non-null child Graph, setting its parent to be this Graph.
		void addChild(Graph* child)
		{
			if (child != nullptr)
			{
                child->setParent(this);
				_children.emplace_back(child);
			}
		}

        //! Remove a non-null child if it exists.
		void removeChild(Graph* child)
		{
			if (auto it = std::find(_children.begin(), _children.end(), child); it != _children.end())
			{
                child->setParent(nullptr);
				_children.erase(it);
			}
		}

        //! \retval nullptr if index >= numChildren()
		Graph* child(std::size_t index)
		{
			if (index < _children.size())
			{
				return _children[index];
			}

			return nullptr;
		}

        std::size_t numChildren() const
        {
            return _children.size();
        }

        //! \return The number of children searching recursively.
        std::size_t numChildrenRecursive() const;

        //! Save this Graph to a buffer.
        dagbase::ByteBuffer* save() const;

        //! Rstore this Graph from a buffer.
        void restore(dagbase::ByteBuffer* memento);

        //! Write this Graph to a stream.
        dagbase::OutputStream& write(dagbase::OutputStream& str, NodeLibrary& nodeLib, Lua& lua) const;

        //! Pretty-print this Graph for debugging purposes.
        void debug() const;

        dagbase::NodeID nextNodeID() override
        {
            return _nextNodeID++;
        }

        dagbase::PortID nextPortID() override
        {
            return _nextPortID++;
        }
    private:
        void setParent(Graph* parent)
        {
            _parent = parent;
        }
		typedef std::unordered_map<std::int64_t, dagbase::Node*> NodeMap;
		NodeMap _nodes;
        typedef std::unordered_map<std::string, dagbase::Node*> NameToNodeMap;
        NameToNodeMap _nodeLookupByName;
		typedef std::unordered_map<std::int64_t, dagbase::SignalPath*> SignalPathMap;
		SignalPathMap _signalPaths;
        typedef std::unordered_map<std::int64_t, dagbase::Port*> PortMap;
        PortMap _ports;
		typedef std::vector<Graph*> GraphArray;
		GraphArray _children;
        dagbase::NodeID _nextNodeID{0};
        dagbase::PortID _nextPortID{0};
        Graph* _parent{nullptr};
        dagbase::NodeLibrary* _nodeLib{nullptr};
		dagbase::Node* _lastAddedNode{ nullptr };

        void readPort(dagbase::Table &portTable, dagbase::Node *node, dagbase::Port *existingPort);
        std::ostream& toLuaHelper(std::ostream & str);
        static Graph* fromLua(dagbase::Lua& lua, dagbase::NodeLibrary& nodeLib);
        static Graph* fromLuaGraphTable(dagbase::Table& graphTable, dagbase::NodeLibrary& nodeLib, Graph* output);
    };
}
