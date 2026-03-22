// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__NODE_REGISTRY_H_
#define BEHAVIOR_FLOW__NODE_REGISTRY_H_

#include <map>
#include <optional>

#include "behavior_flow_node.h"
#include "node_factory.h"
#include "utils/behavior_flow_types.h"
#include "utils/concepts.h"
#include "utils/return_types.h"

namespace bflow {

class NodeRegistry {
 public:
  NodeRegistry();
  // todo: copy/move constructors?

  template <BehaviorFlowNodeType NodeT, typename... Args>
  void registerNodeType(const NodeTypeId& node_type_id, std::vector<ResultId> valid_result_ids,
                        Args&&... args);

  template <typename ResultT, ResultTypeInterpreter<ResultT> ResultTInterpreter, typename Callable>
    requires NodeFunction<Callable, ResultT>
  void registerNodeType(const NodeTypeId& node_type_id, Callable&& execution_function);

  void registerSimpleNodeType(const NodeTypeId& node_type_id,
                              VoidNodeFunction auto&& execution_function);

  void registerSimpleNodeType(const NodeTypeId& node_type_id,
                              SimpleNodeFunction auto&& execution_function);

  void registerDecisionNodeType(const NodeTypeId& node_type_id,
                                BoolNodeFunction auto&& execution_function);

 private:
  void registerMetadata(const NodeTypeMetadata& metadata);
  void registerTerminalNodeType(const NodeTypeId& node_type_id);

  std::unique_ptr<NodeFactory> node_factory_;
  std::map<NodeTypeId, NodeTypeMetadata> node_type_metadata_;

  friend class NodeRegistryAccessor;
};

template <BehaviorFlowNodeType NodeT, typename... Args>
void NodeRegistry::registerNodeType(const NodeTypeId& node_type_id,
                                    std::vector<ResultId> valid_result_ids, Args&&... args) {
  node_factory_->registerNodeType<NodeT>(node_type_id, std::forward<Args>(args)...);
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = std::move(valid_result_ids),
  });
}

template <typename ResultT, ResultTypeInterpreter<ResultT> ResultTInterpreter, typename Callable>
  requires NodeFunction<Callable, ResultT>
void NodeRegistry::registerNodeType(const NodeTypeId& node_type_id, Callable&& execution_function) {
  node_factory_->registerNodeType<BehaviorFlowNode>(
      node_type_id, [fn = std::forward<Callable>(execution_function)]() -> ReturnType {
        return ResultTInterpreter::toNodeReturnType(std::invoke(fn));
      });
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = ResultTInterpreter::validResultIds(),
  });
}

inline void NodeRegistry::registerSimpleNodeType(const NodeTypeId& node_type_id,
                                          VoidNodeFunction auto&& execution_function) {
  node_factory_->registerNodeType<BehaviorFlowNode>(
      node_type_id, [fn = std::forward<decltype(execution_function)>(execution_function)]() {
        std::invoke(fn);
        return ReturnType(SimpleNodeResultId);
      });
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = {SimpleNodeResultId},
  });
}

inline void NodeRegistry::registerSimpleNodeType(const NodeTypeId& node_type_id,
                                          SimpleNodeFunction auto&& execution_function) {
  registerNodeType<SimpleNodeResult, SimpleResultTypeInterpreter>(
      node_type_id, std::forward<decltype(execution_function)>(execution_function));
}

inline void NodeRegistry::registerDecisionNodeType(const NodeTypeId& node_type_id,
                                            BoolNodeFunction auto&& execution_function) {
  registerNodeType<bool, BoolResultTypeInterpreter>(
      node_type_id, std::forward<decltype(execution_function)>(execution_function));
}

// Accessor class for retrieving stuff from NodeRegistry, to keep NodeRegistry's API
// user/register-facing
class NodeRegistryAccessor {
 public:
  static std::unique_ptr<BehaviorFlowNodeBase> instantiateNode(const NodeRegistry& registry,
                                                               const NodeTypeId& node_type_id);
  static std::optional<NodeTypeMetadata> getNodeTypeMetadata(const NodeRegistry& registry,
                                                             const NodeTypeId& node_type_id);
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__NODE_REGISTRY_H_