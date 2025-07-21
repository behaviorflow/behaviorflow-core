// Copyright (c) 2025, Mitch Adams

#include "node_instance_provider.h"

namespace bflow {

NodeInstanceProvider::NodeInstanceProvider(NodeRegistrar registrar) : registrar_(std::move(registrar)) {}

const NodeWithMetadata& NodeInstanceProvider::getNodeInstance(const NodeId& node_instance_id, const NodeTypeId& node_type_id) {
	auto it = node_instance_map_.find(node_instance_id);
	if (it != node_instance_map_.end()) {
		if (it->second.metadata.node_type_id != node_type_id) {
			throw std::runtime_error("Node instance '" + node_instance_id + "' is of type '" +
				it->second.metadata.node_type_id + "' but requested type is '" + node_type_id + "'.");
		}
		return it->second;
	}
	auto node = NodeRegistrarInstantiator::instantiateNode(registrar_, node_type_id, node_instance_id);
	node_instance_map_[node_instance_id] = std::move(node);
	return node_instance_map_[node_instance_id];
}


} // namespace bflow