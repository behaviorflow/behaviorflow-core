// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_GRAPH_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_GRAPH_H_

#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "utils/behavior_flow_types.h"
#include "utils/behavior_flow_utils.h"

namespace bflow {

class NodeGraph {
 public:
  struct NodeDescription {
    NodeId node_id;
    NodeTypeId node_type;
    std::map<ResultId, NodeId> transitions;
  };

  struct NodeTypeDescription {
    NodeTypeId node_type_id;
    std::vector<ResultId> result_ids;
  };

  NodeGraph() = default;
  NodeGraph(const std::vector<NodeTypeDescription>& node_types,
            const std::vector<NodeDescription>& nodes, const NodeId& start_node_id);

  void addNodeType(const NodeTypeDescription& node_type_description);
  void addStartNode(const NodeDescription& node_description);
  void addNode(const NodeDescription& node_description);
  NodeDescription getStartNode() const;
  NodeDescription getNextNode(const NodeId& from_node_id, const ResultId& result_id) const;
  void validateThatGraphIsComplete() const;
  const std::unordered_map<NodeId, NodeDescription>& getAllNodes() const;
  const std::unordered_map<NodeTypeId, NodeTypeDescription>& getAllNodeTypes() const;

 private:
  void validateNodeExists(const NodeId& node_id, const std::string& exception_msg) const;
  void validateTransitionExists(const NodeId& from_node_id, const ResultId& result_id,
                                const std::string& exception_msg) const;
  void validateAllTransitionedToNodesExist() const;
  void validateAllNodesAreReachableFromStart() const;

  NodeId start_node_id_;
  std::unordered_map<NodeId, NodeDescription> nodes_;
  std::unordered_map<NodeTypeId, NodeTypeDescription> node_types_;
};

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_GRAPH_H_