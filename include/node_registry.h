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

/**
 * @brief Maintains a registry of node types and their metadata.
 */
class NodeRegistry {
 public:
  NodeRegistry();
  // todo: copy/move constructors?

  /**
   * @brief Registers a node type so that it can be used in a node graph. Registered node types
   * define the behavior of node instances associated with them.
   * @tparam NodeT The type of the node to register. Must derive from BehaviorFlowNodeBase.
   * @tparam Args The constructor argument types of NoteT.
   * @param node_type_id The unique identifier used to reference this node type.
   * @param valid_result_ids The list of all result IDs that this node can return after executing.
   * The result IDs are used to determine the next node to execute based on the result of this node.
   * @param args What arguments to pass to the constructor of the node type whenever instantiated.
   */
  template <BehaviorFlowNodeType NodeT, typename... Args>
  void registerNodeType(const NodeTypeId& node_type_id, std::vector<ResultId> valid_result_ids,
                        Args&&... args);

  /**
   * @brief Registers a node type defined by a given execution function and result type interpreter.
   * Registered node types can be used in a node graph. They define the behavior of node instances
   * associated with the node type.
   * @tparam ResultT The return type of the execution function.
   * @tparam ResultTInterpreter Defines how the return type of the execution function is translated
   * into a standard ReturnType type. See include/utils/concepts.h
   * @tparam Callable The type of the execution function. See include/utils/concepts.h for valid
   * Callable types
   * @param node_type_id The unique identifier used to reference this node type.
   * @param execution_function The function to execute when a node of this type is run.
   * @param result_interpreter The instance responsible for interpreting the result of the execution
   * function.
   */
  template <typename ResultT, ResultTypeInterpreter<ResultT> ResultTInterpreter, typename Callable>
    requires NodeFunction<Callable, ResultT>
  void registerNodeType(const NodeTypeId& node_type_id, Callable&& execution_function);

  /**
   * @brief Registers a node type that has an execution function with void return type (suggesting
   * that the node lasts a single tick and that there is only one possible node it can transition
   * to).
   */
  void registerSimpleNodeType(const NodeTypeId& node_type_id,
                              VoidNodeFunction auto&& execution_function);

  /**
   * @brief Registers a node type that has an execution function that returns a SimpleNodeResult,
   * meaning that after each execution tick the node can either be completed or still running. If it
   * is still running, it will continue to be executed in subsequent ticks. If it is completed, it
   * will transition to the next node.
   */
  void registerSimpleNodeType(const NodeTypeId& node_type_id,
                              SimpleNodeFunction auto&& execution_function);

  /**
   * @brief Registers a node type that has an execution function with a boolean return type. The
   * return value will determine the flow of execution.
   */
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
      node_type_id,
      [fn = std::forward<Callable>(execution_function)]() mutable
          -> ReturnType {  // todo: consider removing mutable and make move-to functors template
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
      node_type_id,
      [fn =
           std::forward<decltype(execution_function)>(execution_function)]() mutable -> ReturnType {
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

/** Accessor class for instantiating nodes from NodeRegistry to keep NodeRegistry's API
 * user/register-facing */
class NodeRegistryAccessor {
 public:
  static std::unique_ptr<BehaviorFlowNodeBase> instantiateNode(const NodeRegistry& registry,
                                                               const NodeTypeId& node_type_id);
  static std::optional<NodeTypeMetadata> getNodeTypeMetadata(const NodeRegistry& registry,
                                                             const NodeTypeId& node_type_id);
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__NODE_REGISTRY_H_