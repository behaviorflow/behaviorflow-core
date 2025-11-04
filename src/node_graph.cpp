// Copyright (c) 2025, Mitch Adams

#include "node_graph.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stack>
#include <unordered_set>

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
                             "' as a node type with that id already exists in the graph.");
  }
  node_types_.insert({node_type_description.node_type_id, node_type_description});
}

void NodeGraph::addStartNode(const NodeDescription& node_description) {
  if (!start_node_id_.empty()) {  // assuming existing start node id is not empty string
    throw std::runtime_error("Cannot add start node '" + node_description.node_id +
                             "' as a start node already exists in the graph.");
  }
  addNode(node_description);
  start_node_id_ = node_description.node_id;
}

void NodeGraph::addNode(const NodeDescription& node_description) {
  if (nodes_.find(node_description.node_id) != nodes_.end()) {
    throw std::runtime_error("Cannot add node '" + node_description.node_id +
                             "' as a node with that id already exists in the graph.");
  }
  auto node_type_description_it = node_types_.find(node_description.node_type);
  if (node_type_description_it == node_types_.end()) {
    throw std::runtime_error("Cannot add node '" + node_description.node_id + "' of type '" +
                             node_description.node_type +
                             "' as that node type has not been defined for this graph.");
  } else {
    const NodeTypeDescription& node_type_description = node_type_description_it->second;
    auto transition_result_ids = node_description.transitions | std::views::keys;
    if (!std::ranges::is_permutation(transition_result_ids, node_type_description.result_ids)) {
      throw std::runtime_error("Cannot add node '" + node_description.node_id +
                               "' as its transition result ids do not match "
                               "the result ids for its node type '" +
                               node_description.node_type + "'.");
    }
  }
  nodes_.insert({node_description.node_id, node_description});
}

NodeGraph::NodeDescription NodeGraph::getStartNode() const { return nodes_.at(start_node_id_); }

NodeGraph::NodeDescription NodeGraph::getNextNode(const NodeId& from_node_id,
                                                  const ResultId& result_id) const {
  validateNodeExists(from_node_id, "Cannot get the next node from '" + from_node_id +
                                       "' as it does not exist in the graph.");
  validateTransitionExists(from_node_id, result_id,
                           "Cannot get the next node from '" + from_node_id + "' for transition '" +
                               result_id +
                               "' as "
                               "the transition does not exist for that node.");
  NodeDescription from_node = nodes_.at(from_node_id);
  NodeId next_node_id = from_node.transitions.at(result_id);
  validateNodeExists(next_node_id, "Cannot get the next node from '" + from_node_id +
                                       "' for transition '" + result_id +
                                       "' as the "
                                       "next node '" +
                                       next_node_id + "' does not exist in the graph.");
  return nodes_.at(next_node_id);
}

void NodeGraph::validateThatGraphIsComplete() const {
  if (start_node_id_ == "") {
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
      error_msg_stream << "  * Node \"" << node_id << "\" connects to nonexistant node(s): ";
      bool first = true;
      for (const auto& target : nonexistant_transition_range) {
        if (!first) error_msg_stream << ", ";
        error_msg_stream << "\"" << target << "\"";
        first = false;
      }
      error_msg_stream << "\n";
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
    std::ostringstream error_msg_stream;
    error_msg_stream << "Graph validation failed: Unreachable node(s): ";
    for (const auto& id : unreachable) error_msg_stream << "'" << id << "', ";
    std::string error_msg = error_msg_stream.str();
    error_msg = error_msg.substr(0, error_msg.size() - 2);
    throw std::runtime_error(error_msg);
  }
}

}  // end namespace bflow