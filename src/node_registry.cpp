// Copyright (c) 2025, Mitch Adams

#include "node_registry.h"
#include "standard_node_library.h"

namespace bflow {

NodeRegistry::NodeRegistry() : node_factory_(std::make_unique<NodeFactory>()) {
  register_standard_library(*this);
};

void NodeRegistry::registerMetadata(const NodeTypeId& node_type_id) {
  if (node_type_metadata_.find(node_type_id) != node_type_metadata_.end()) {
    throw std::runtime_error("Node type with id '" + node_type_id + "' is already registered.");
  }
  node_type_metadata_[node_type_id] = NodeTypeMetadata{.node_type_id = node_type_id};
}

NodeWithMetadata NodeRegistryInstantiator::instantiateNode(const NodeRegistry& registry,
                                                           const NodeTypeId& node_type_id) {
  auto metadata_it = registry.node_type_metadata_.find(node_type_id);
  if (metadata_it == registry.node_type_metadata_.end()) {
    throw std::runtime_error("Node type with id '" + node_type_id + "' has not been registered.");
  }
  NodeTypeMetadata metadata = metadata_it->second;
  std::unique_ptr<BehaviorFlowNodeBase> node_instance =
      registry.node_factory_->createNodeInstance(node_type_id);
  return NodeWithMetadata{.metadata = metadata, .node_instance = std::move(node_instance)};
}

}  // namespace bflow