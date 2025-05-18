// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__NODE_REGISTRAR_H_
#define BEHAVIOR_FLOW__NODE_REGISTRAR_H_

#include "behavior_flow_node.h"
#include "node_factory.h"

namespace bflow
{

template <typename ReturnT, typename ReturnTInterpreter>
void registerNodeType(
	const std::string& node_type_id,
	const std::string& node_type_description,
	std::function<ReturnT()> execution_function)
{

	// NodeFactory::getInstance().registerNodeType(
	// 	node_type_id,
	// 	node_type_description,
	// 	[execution_function]() {
	// 		return std::make_shared<ReturnTInterpreter>(execution_function());
	// 	}
	// );
}

void registerSimpleNodeType(
	const std::string& node_type_id,
	std::function<void()> execution_function)
{
	NodeFactory::getInstance().registerNodeType<SimpleBehaviorFlowNode>(node_type_id, execution_function);
}


} // namespace bflow

#endif // BEHAVIOR_FLOW__NODE_REGISTRAR_H_