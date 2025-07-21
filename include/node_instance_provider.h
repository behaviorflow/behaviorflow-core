// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__NODE_EXECUTOR_H_
#define BEHAVIOR_FLOW__NODE_EXECUTOR_H_

#include <string>
#include <unordered_map>
#include <memory>
#include "behavior_flow_node.h"
#include "node_factory.h"
#include "node_registrar.h"
#include "utils/behavior_flow_utils.h"

namespace bflow
{

class NodeInstanceProvider {
public:
	explicit NodeInstanceProvider(NodeRegistrar registrar);
	NodeInstanceProvider() = delete;;
	const NodeWithMetadata& getNodeInstance(const NodeId& node_instance_id, const NodeTypeId& node_type_id);
	
private:
	std::unordered_map<NodeId, NodeWithMetadata> node_instance_map_;
	std::unordered_map<NodeId, NodeTypeId> node_type_map_;
	NodeRegistrar registrar_;
};

} // namespace bflow

#endif  // BEHAVIOR_FLOW__NODE_EXECUTOR_H_