// Copyright (c) 2025, Mitch Adams

#include "node_graph.h"

#include <algorithm>
#include <ranges>
#include <sstream>
#include <stack>
#include <unordered_set>

#include "utils/behavior_flow_utils.h"

namespace bflow {

NodeGraph::NodeGraph(const std::vector<NodeTypeDescription>& node_types,
                     const std::vector<NodeDescription>& nodes, const NodeId& start_node_id) {
  bool start_node_found = false;
  for (const auto& node_type : node_types) {
    addNodeType(node_type);
  }
  for (const auto& node : nodes) {
    if (node.node_id == start_node_id) {
      addStartNode(node);
      start_node_found = true;
    } else {
      addNode(node);
    }
  }
  if (!start_node_found) {
    throw std::runtime_error("Cannot construct NodeGraph: Start node with id '" + start_node_id +
                             "' not found in provided nodes.");
  }
  validateThatGraphIsComplete();
}

void NodeGraph::addNodeType(const NodeTypeDescription& node_type_description) {
  if (node_types_.find(node_type_description.node_type_id) != node_types_.end()) {
    throw std::runtime_error("Cannot add node type '" + node_type_description.node_type_id +
                             "' because a node type with that id already exists in the graph.");
  }
  node_types_.insert({node_type_description.node_type_id, node_type_description});
}

void NodeGraph::addStartNode(const NodeDescription& node_description) {
  if (!start_node_id_.empty()) {
    throw std::runtime_error("Cannot add start node '" + node_description.node_id +
                             "' because a start node already exists in the graph.");
  }
  addNode(node_description);
  start_node_id_ = node_description.node_id;
}

void NodeGraph::addNode(const NodeDescription& node_description) {
  if (node_description.node_id.empty()) {
    throw std::runtime_error("Cannot add a node with an empty node id.");
  }
  if (nodes_.find(node_description.node_id) != nodes_.end()) {
    throw std::runtime_error("Cannot add node '" + node_description.node_id +
                             "' because a node with that id already exists in the graph.");
  }
  auto node_type_description_it = node_types_.find(node_description.node_type);
  if (node_type_description_it == node_types_.end()) {
    throw std::runtime_error("Cannot add node '" + node_description.node_id + "' of type '" +
                             node_description.node_type +
                             "' because that node type has not been defined for this graph.");
  } else {
    const NodeTypeDescription& node_type_description = node_type_description_it->second;
    auto transition_result_ids = node_description.transitions | std::views::keys;
    if (!std::ranges::is_permutation(transition_result_ids, node_type_description.result_ids)) {
      throw std::runtime_error("Cannot add node '" + node_description.node_id +
                               "' because its transition result ids (" +
                               joinStrings(transition_result_ids, ", ") +
                               ") do not match "
                               "the result ids for its node type, '" +
                               node_description.node_type + "', (" +
                               joinStrings(node_type_description.result_ids, ", ") + ").");
    }
  }
  nodes_.insert({node_description.node_id, node_description});
}

NodeGraph::NodeDescription NodeGraph::getStartNode() const { return nodes_.at(start_node_id_); }

NodeGraph::NodeDescription NodeGraph::getNextNode(const NodeId& from_node_id,
                                                  const ResultId& result_id) const {
  validateNodeExists(from_node_id, "Cannot get the next node from '" + from_node_id +
                                       "' because it does not exist in the graph.");
  validateTransitionExists(from_node_id, result_id,
                           "Cannot get the next node from '" + from_node_id + "' for transition '" +
                               result_id +
                               "' because the transition does not exist for that node.");
  NodeDescription from_node = nodes_.at(from_node_id);
  NodeId next_node_id = from_node.transitions.at(result_id);
  validateNodeExists(next_node_id, "Cannot get the next node from '" + from_node_id +
                                       "' for transition '" + result_id +
                                       "' because the next node '" + next_node_id +
                                       "' does not exist in the graph.");
  return nodes_.at(next_node_id);
}

void NodeGraph::validateThatGraphIsComplete() const {
  if (start_node_id_.empty()) {
    throw std::runtime_error("Graph validation failed: No start node defined.");
  }
  validateAllTransitionedToNodesExist();
  validateAllNodesAreReachableFromStart();
}

void NodeGraph::validateNodeExists(const NodeId& node_id, const std::string& exception_msg) const {
  if (!nodes_.contains(node_id)) {
    throw std::runtime_error(exception_msg);
  }
}

void NodeGraph::validateTransitionExists(const NodeId& from_node_id, const ResultId& result_id,
                                         const std::string& exception_msg) const {
  validateNodeExists(from_node_id, exception_msg);
  const auto& from_node = nodes_.at(from_node_id);
  if (!from_node.transitions.contains(result_id)) {
    throw std::runtime_error(exception_msg);
  }
}

void NodeGraph::validateAllTransitionedToNodesExist() const {
  std::ostringstream error_msg_stream;
  bool found_invalid = false;
  for (const auto& [node_id, node_desc] : nodes_) {
    auto nonexistant_transition_range =
        node_desc.transitions | std::views::values |
        std::views::filter([this](const NodeId& target) { return !nodes_.contains(target); });
    if (!std::ranges::empty(nonexistant_transition_range)) {
      if (!found_invalid) {
        error_msg_stream << "Graph validation failed. The following nodes transition to nodes that "
                            "are not defined within the node graph:\n";
        found_invalid = true;
      }
      error_msg_stream << "  * Node \"" << node_id << "\" connects to nonexistant node(s): "
                       << joinStrings(nonexistant_transition_range, ", ") << "\n";
    }
  }
  if (found_invalid) {
    throw std::runtime_error(error_msg_stream.str());
  }
}

void NodeGraph::validateAllNodesAreReachableFromStart() const {
  std::unordered_set<NodeId> visited;
  std::stack<NodeId> to_visit;
  to_visit.push(start_node_id_);
  while (!to_visit.empty()) {
    NodeId current = to_visit.top();
    to_visit.pop();
    if (!visited.insert(current).second) continue;
    for (const auto& [_, next] : nodes_.at(current).transitions) {
      to_visit.push(next);
    }
  }
  auto unreachable = nodes_ | std::views::keys | std::views::filter([&visited](const NodeId& id) {
                       return !visited.contains(id);
                     });
  if (!std::ranges::empty(unreachable)) {
    throw std::runtime_error("Graph validation failed: Unreachable node(s): " +
                             joinStrings(unreachable, ", "));
  }
}

const std::unordered_map<NodeId, NodeGraph::NodeDescription>& NodeGraph::getAllNodes() const {
  return nodes_;
}

const std::unordered_map<NodeTypeId, NodeGraph::NodeTypeDescription>& NodeGraph::getAllNodeTypes()
    const {
  return node_types_;
}

}  // end namespace bflow