// Copyright (c) 2025, Mitch Adams

#include "node_instance_provider.h"

namespace bflow {

NodeInstanceProvider::NodeInstanceProvider(NodeRegistry&& registry)
    : registry_(std::move(registry)) {}

BehaviorFlowNodeBase& NodeInstanceProvider::getNodeInstance(
  const NodeId& node_instance_id, const NodeTypeId& node_type_id) {
  auto it = node_instance_map_.find(node_instance_id);
  if (it != node_instance_map_.end()) {
    if (it->second.first != node_type_id) {
      throw std::runtime_error("Node instance '" + node_instance_id + "' is of type '" +
                               it->second.first + "' but requested type is '" + node_type_id +
                               "'.");
    }
    return *(it->second.second);
  }
  // Node instance does not exist, so let's create it
  auto node = NodeRegistryAccessor::instantiateNode(registry_, node_type_id);
  if (!node) {
    throw std::runtime_error("Unable to instantiate node instance '" + node_instance_id +
                             "' of type '" + node_type_id +
                             "'. It may be that this node type was never registered.");
  }
  node_instance_map_[node_instance_id] = std::make_pair(node_type_id, std::move(node));
  return *(node_instance_map_[node_instance_id].second);
}

std::optional<NodeTypeMetadata> NodeInstanceProvider::getNodeTypeMetadata(
    const NodeTypeId& node_type_id) const {
  return NodeRegistryAccessor::getNodeTypeMetadata(registry_, node_type_id);
}

}  // namespace bflow