// Copyright (c) 2025, Mitch Adams

#include "node_registrar.h"

namespace bflow {

void NodeRegistrar::registerMetadata(const NodeTypeId& node_type_id)
{
	if (node_type_metadata_.find(node_type_id) != node_type_metadata_.end()) {
		throw std::runtime_error("Node type with id '" + node_type_id + "' is already registered.");
	}
	node_type_metadata_[node_type_id] = NodeTypeMetadata{.node_type_id = node_type_id};
}

NodeWithMetadata NodeRegistrarInstantiator::instantiateNode(const NodeRegistrar& registrar, const NodeTypeId& node_type_id, const NodeId& node_instance_id)
{
	auto metadata_it = registrar.node_type_metadata_.find(node_type_id);
	if (metadata_it == registrar.node_type_metadata_.end()) {
		throw std::runtime_error("Node type with id '" + node_type_id + "' is not registered.");
	}
	NodeTypeMetadata metadata = metadata_it->second;
	metadata.node_instance_id = node_instance_id;
	std::unique_ptr<BehaviorFlowNodeBase> node_instance = registrar.node_factory_->createNodeInstance(node_type_id);
	return NodeWithMetadata{.metadata = metadata, .node_instance = std::move(node_instance)};
}

} // namespace bflow