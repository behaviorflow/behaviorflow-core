// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_GRAPH_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_GRAPH_H_

#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace bflow {

class NodeGraph {
 public:
  using NodeId = std::string;
  using NodeTypeId = std::string;
  using ResultId = std::string;

  struct NodeDescription {
    NodeId node_id;
    NodeTypeId node_type;
    std::map<ResultId, NodeId> transitions;
  };

  NodeGraph() = default;
  void addStartNode(const NodeDescription& node_description);
  void addNode(const NodeDescription& node_description);
  NodeDescription getStartNode() const;
  NodeDescription getNextNode(const NodeId& from_node_id, const ResultId& result_id) const;
  bool allTransitionedToNodesExist();

 private:
  void validateNodeExists(const NodeId& node_id, const std::string& exception_msg) const;
  void validateTransitionExists(const NodeId& from_node_id, const ResultId& result_id,
                                const std::string& exception_msg) const;

  NodeId start_node_id_;
  std::unordered_map<NodeId, NodeDescription> nodes_;
};

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_GRAPH_H_