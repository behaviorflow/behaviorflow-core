// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__NODE_REGISTRY_H_
#define BEHAVIOR_FLOW__NODE_REGISTRY_H_

#include <map>

#include "behavior_flow_node.h"
#include "node_factory.h"
#include "utils/behavior_flow_types.h"

namespace bflow {

class NodeRegistry {
 public:
  NodeRegistry();

  void registerSimpleNodeType(const NodeTypeId& node_type_id,
                              std::function<void()> execution_function) {
    node_factory_->registerNodeType<SimpleBehaviorFlowNode>(node_type_id, execution_function);
    registerMetadata(node_type_id);
  }

 private:
  void registerMetadata(const NodeTypeId& node_type_id);  // this exists assuming metadata will
                                                          // expand to more than just the node ID.

  std::unique_ptr<NodeFactory> node_factory_;
  std::map<NodeTypeId, NodeTypeMetadata> node_type_metadata_;

  friend class NodeRegistryInstantiator;
};

class NodeRegistryInstantiator {
 public:
  static NodeWithMetadata instantiateNode(const NodeRegistry& registry,
                                          const NodeTypeId& node_type_id,
                                          const NodeId& node_instance_id);
};

// template <typename ReturnT, typename ReturnTInterpreter>
// void registerNodeType(
// 	const std::string& node_type_id,
// 	// const std::string& node_type_description,
// 	std::function<ReturnT()> execution_function)
// {

// 	// NodeFactory::getInstance().registerNodeType(
// 	// 	node_type_id,
// 	// 	node_type_description,
// 	// 	[execution_function]() {
// 	// 		return std::make_shared<ReturnTInterpreter>(execution_function());
// 	// 	}
// 	// );
// }

// void registerSimpleNodeType(
// 	const std::string& node_type_id,
// 	void(*execution_function)())
// {
// 	NodeFactory::getInstance().registerNodeType<SimpleBehaviorFlowNode>(node_type_id,
// execution_function);
// }

// template<typename Callable>
// void registerSimpleNodeType(
// 	const std::string& node_type_id,
// 	Callable&& execution_function)
// {
// 	NodeFactory::getInstance().registerNodeType<SimpleBehaviorFlowNode>(
// 		node_type_id,
// 		std::forward<Callable>(execution_function)
// 	);
// }

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__NODE_REGISTRY_H_