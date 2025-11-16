// Copyright (c) 2025, Mitch Adams

#include "node_registry.h"

#include "standard_node_library.h"
#include "utils/behavior_flow_utils.h"
#include <optional>

namespace bflow {

NodeRegistry::NodeRegistry() : node_factory_(std::make_unique<NodeFactory>()) {
  register_standard_library(*this);
  registerTerminalNodeType(SuccessNodeTypeId);
  registerTerminalNodeType(FailureNodeTypeId);
};

void NodeRegistry::registerSimpleNodeType(const NodeTypeId& node_type_id,
                                          std::function<void()> execution_function) {
  node_factory_->registerNodeType<SimpleBehaviorFlowNode>(node_type_id, execution_function);
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = {""},
  });
};

void NodeRegistry::registerTerminalNodeType(const NodeTypeId& node_type_id) {
  node_factory_->registerNodeType<SimpleBehaviorFlowNode>(node_type_id, []() -> void {});
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = {},
  });
}

void NodeRegistry::registerMetadata(const NodeTypeMetadata& metadata) {
  if (node_type_metadata_.find(metadata.node_type_id) != node_type_metadata_.end()) {
    throw std::runtime_error("Node type with id '" + metadata.node_type_id +
                             "' is already registered.");
  }
  node_type_metadata_[metadata.node_type_id] = metadata;
}

NodeWithMetadata NodeRegistryInstantiator::instantiateNode(const NodeRegistry& registry,
                                                           const NodeTypeId& node_type_id) {
  auto metadata_it = registry.node_type_metadata_.find(node_type_id);
  if (metadata_it == registry.node_type_metadata_.end()) {
    throw std::runtime_error("Unable to instantiate node with node type '" + node_type_id +
                             "' because no such node type has been registered.");
  }
  NodeTypeMetadata metadata = metadata_it->second;
  std::unique_ptr<BehaviorFlowNodeBase> node_instance =
      registry.node_factory_->createNodeInstance(node_type_id);
  return NodeWithMetadata{.metadata = metadata, .node_instance = std::move(node_instance)};
}

std::optional<NodeTypeMetadata> NodeTypeMetadataViewer::getNodeTypeMetadata(
    const NodeRegistry& registry, const NodeTypeId& node_type_id) {
  auto metadata_it = registry.node_type_metadata_.find(node_type_id);
  if (metadata_it == registry.node_type_metadata_.end()) {
    return std::nullopt;
  }
  return metadata_it->second;
}

}  // namespace bflow