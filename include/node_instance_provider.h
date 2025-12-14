// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__NODE_EXECUTOR_H_
#define BEHAVIOR_FLOW__NODE_EXECUTOR_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "behavior_flow_node.h"
#include "node_factory.h"
#include "node_registry.h"
#include "utils/behavior_flow_types.h"

namespace bflow {

class NodeInstanceProvider {
 public:
  explicit NodeInstanceProvider(NodeRegistry&& registry);
  NodeInstanceProvider() = default;

  BehaviorFlowNodeBase& getNodeInstance(const NodeId& node_instance_id,
                                          const NodeTypeId& node_type_id);

  std::optional<NodeTypeMetadata> getNodeTypeMetadata(const NodeTypeId& node_type_id) const;

  void clear() { node_instance_map_.clear(); }

 private:
  std::unordered_map<NodeId, std::pair<NodeTypeId, std::unique_ptr<BehaviorFlowNodeBase>>> node_instance_map_;
  NodeRegistry registry_;
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__NODE_EXECUTOR_H_