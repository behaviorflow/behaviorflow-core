#include "node_graph.h"

namespace bflow {

void NodeGraph::addStartNode(const NodeDescription& node_description) {
  if (start_node_id_ != "") {  // assuming existing start node id is not empty string
    throw std::runtime_error("Cannot add start node '" + node_description.node_id +
                             "' as a start node already exists in the graph.");
  }
  addNode(node_description);
  start_node_id_ = node_description.node_id;
}

void NodeGraph::addNode(const NodeDescription& node_description) {
  if (nodes_.find(node_description.node_id) != nodes_.end()) {
    throw std::runtime_error("Cannot add node '" + node_description.node_id +
                             "' as it already exists in the graph.");
  }
  nodes_.insert({node_description.node_id, node_description});
}

NodeGraph::NodeDescription NodeGraph::getStartNode() const { return nodes_.at(start_node_id_); }

NodeGraph::NodeDescription NodeGraph::getNextNode(const NodeId& from_node_id,
                                                  const ResultId& result_id) const {
  validateNodeExists(from_node_id, "Cannot get the next node from '" + from_node_id +
                                       "'. Node does not exist in the graph.");
  validateTransitionExists(from_node_id, result_id,
                           "Cannot get the next node from '" + from_node_id + "' for transition '" +
                               result_id + "', as the transition does not exist for that node.");
  NodeDescription from_node = nodes_.at(from_node_id);
  NodeId next_node_id = from_node.transitions.at(result_id);
  validateNodeExists(next_node_id, "Cannot get the next node from '" + from_node_id +
                                       "' for transition '" + result_id + "', as the next node '" +
                                       next_node_id + "' does not exist in the graph.");
  return nodes_.at(next_node_id);
}

void NodeGraph::validateNodeExists(const NodeId& node_id, const std::string& exception_msg) const {
  if (nodes_.find(node_id) == nodes_.end()) {
    throw std::runtime_error(exception_msg);
  }
}

void NodeGraph::validateTransitionExists(const NodeId& from_node_id, const ResultId& result_id,
                                         const std::string& exception_msg) const {
  validateNodeExists(from_node_id, exception_msg);
  NodeDescription from_node = nodes_.at(from_node_id);
  if (from_node.transitions.find(result_id) == from_node.transitions.end()) {
    throw std::runtime_error(exception_msg);
  }
}

bool NodeGraph::allTransitionedToNodesExist() {
  for (const auto& node : nodes_) {
    for (const auto& transition : node.second.transitions) {
      if (nodes_.find(transition.second) == nodes_.end()) {
        return false;
      }
    }
  }
  return true;
}

}  // end namespace bflow