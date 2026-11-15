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
   * define the behavior of node instances associated with them. A BehaviorFlowNodeType object will
   * be created for each node instance.
   * @tparam NodeT The type of the node to register. Must derive from BehaviorFlowNodeBase.
   * @tparam Args The constructor argument types of NoteT.
   * @param node_type_id The unique identifier used to reference this node type.
   * @param valid_result_ids The list of all result IDs that this node can return after executing.
   * The result IDs are used to determine the next node to execute based on the result of this node.
   * @param constructor_args What arguments to pass to the constructor of the node type whenever
   * instantiated. These will be copied for each node instance (and thus must be copiable).
   */
  template <BehaviorFlowNodeType NodeT, typename... Args>
  void registerNodeType(const NodeTypeId& node_type_id,
                        std::vector<ResultId> valid_result_ids,
                        Args&&... constructor_args);

  /**
   * @brief Registers a node type defined by a given execution tick function and return type
   * interpreter. Registered node types can be used in a node graph. They define the behavior of
   * node instances associated with the node type.
   * @tparam ReturnT The return type of the execution tick function.
   * @tparam ReturnTInterpreter Defines how the return type of the execution tick function is
   * translated into a standard NodeTickResult type. See include/utils/concepts.h for required
   * interface.
   * @tparam Callable The type of the execution tick function. See include/utils/concepts.h for
   * valid Callable types
   * @param node_type_id The unique identifier used to reference this node type.
   * @param execution_tick_function Called one or more times until the node finishes execution.
   */
  template <typename ReturnT, ReturnTypeInterpreter<ReturnT> ReturnTInterpreter, typename Callable>
    requires NodeFunction<Callable, ReturnT>
  void registerNodeType(const NodeTypeId& node_type_id, Callable&& execution_tick_function);

  /**
   * @brief
   * @tparam ObjectT
   * @tparam ...CtorArgs
   * @tparam ReturnTInterpreter
   * @tparam InitFcn
   * @tparam TeardownFcn
   * @tparam TickFcn
   * @param node_type_id
   * @param ...constructor_args
   */
  template <typename ObjectT,
            auto TickFcn,
            typename ReturnTInterpreter,
            auto InitFcn = nullptr,
            auto TeardownFcn = nullptr,
            typename... CtorArgs>
    requires ValidObjectWrapperNode<ObjectT, TickFcn, ReturnTInterpreter, InitFcn, TeardownFcn>
  void registerNodeType(const NodeTypeId& node_type_id, CtorArgs&&... constructor_args);

  /**
   * @brief Registers a simple node type (meaning that there is only one output port) using an
   * execution function with void return type.
   */
  void registerSimpleNodeType(const NodeTypeId& node_type_id,
                              VoidNodeFunction auto&& execution_function);

  /**
   * @brief Registers a simple node type (meaning that there is only one output port) using an
   * execution tick function with a AsyncSimpleNodeResult return type. The AsyncSimpleNodeResult
   * type allows the user to specify that the node is still running after each tick,
   * meaning it will be ticked again until the node is no longer running.
   */
  void registerSimpleNodeTypeAsync(const NodeTypeId& node_type_id,
                                   AsyncSimpleNodeFunction auto&& execution_tick_function);

  /**
   * @brief
   * @tparam ObjectT
   * @tparam ...CtorArgs
   * @tparam TickFcn
   * @tparam InitFcn
   * @tparam TeardownFcn
   * @param node_type_id
   * @param ...constructor_args
   */
  template <typename ObjectT,
            auto TickFcn,
            auto InitFcn = nullptr,
            auto TeardownFcn = nullptr,
            typename... CtorArgs>
    requires ValidObjectWrapperNode<ObjectT,
                                    TickFcn,
                                    AsyncSimpleNodeReturnTypeInterpreter,
                                    InitFcn,
                                    TeardownFcn>
  void registerSimpleNodeTypeAsync(const NodeTypeId& node_type_id, CtorArgs&&... constructor_args);

  /**
   * @brief Registers a decision node type (Yes or No ports) that has an execution function with a
   * boolean return type. True maps to "Yes" and False maps to "No".
   */
  void registerDecisionNodeType(const NodeTypeId& node_type_id,
                                BoolNodeFunction auto&& execution_function);

  /**
   * @brief
   * @tparam ObjectT
   * @tparam ...CtorArgs
   * @tparam ExecutionFcn
   * @tparam InitFcn
   * @tparam TeardownFcn
   * @param node_type_id
   * @param ...constructor_args
   */
  template <typename ObjectT,
            auto ExecutionFcn,
            auto InitFcn = nullptr,
            auto TeardownFcn = nullptr,
            typename... CtorArgs>
    requires ValidObjectWrapperNode<ObjectT,
                                    ExecutionFcn,
                                    BoolReturnTypeInterpreter,
                                    InitFcn,
                                    TeardownFcn>
  void registerDecisionNodeType(const NodeTypeId& node_type_id, CtorArgs&&... constructor_args);

 private:
  void registerMetadata(const NodeTypeMetadata& metadata);
  void registerTerminalNodeType(const NodeTypeId& node_type_id);

  std::unique_ptr<NodeFactory> node_factory_;
  std::map<NodeTypeId, NodeTypeMetadata> node_type_metadata_;

  friend class NodeRegistryAccessor;
};

template <BehaviorFlowNodeType NodeT, typename... Args>
void NodeRegistry::registerNodeType(const NodeTypeId& node_type_id,
                                    std::vector<ResultId> valid_result_ids,
                                    Args&&... constructor_args) {
  node_factory_->registerNodeType<NodeT>(node_type_id, std::forward<Args>(constructor_args)...);
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = std::move(valid_result_ids),
  });
}

template <typename ReturnT, ReturnTypeInterpreter<ReturnT> ReturnTInterpreter, typename Callable>
  requires NodeFunction<Callable, ReturnT>
void NodeRegistry::registerNodeType(const NodeTypeId& node_type_id,
                                    Callable&& execution_tick_function) {
  node_factory_->registerNodeType<BehaviorFlowNode>(
      node_type_id, [fn = std::forward<Callable>(execution_tick_function)]() mutable -> NodeTickResult {
        return ReturnTInterpreter::toNodeTickResult(std::invoke(fn));
      });
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = ReturnTInterpreter::validResultIds(),
  });
}

template <typename ObjectT,
          auto TickFcn,
          typename ReturnTInterpreter,
          auto InitFcn = nullptr,
          auto TeardownFcn = nullptr,
          typename... CtorArgs>
  requires ValidObjectWrapperNode<ObjectT, TickFcn, ReturnTInterpreter, InitFcn, TeardownFcn>
void NodeRegistry::registerNodeType(const NodeTypeId& node_type_id,
                                    CtorArgs&&... constructor_args) {
  registerNodeType<BehaviorFlowNodeObjectWrapper<ObjectT, TickFcn, ReturnTInterpreter, InitFcn,
                                                 TeardownFcn>>(
      node_type_id, ReturnTInterpreter::validResultIds(),
      std::forward<CtorArgs>(constructor_args)...);
}

inline void NodeRegistry::registerSimpleNodeType(const NodeTypeId& node_type_id,
                                                 VoidNodeFunction auto&& execution_function) {
  node_factory_->registerNodeType<BehaviorFlowNode>(
      node_type_id,
      [fn = std::forward<decltype(execution_function)>(execution_function)]() -> NodeTickResult {
        std::invoke(fn);
        return NodeTickResult(SimpleNodeResultId);
      });
  registerMetadata(NodeTypeMetadata{
      .node_type_id = node_type_id,
      .valid_result_ids = {SimpleNodeResultId},
  });
}

inline void NodeRegistry::registerSimpleNodeTypeAsync(
    const NodeTypeId& node_type_id, AsyncSimpleNodeFunction auto&& execution_tick_function) {
  registerNodeType<AsyncSimpleNodeResult, AsyncSimpleNodeReturnTypeInterpreter>(
      node_type_id, std::forward<decltype(execution_tick_function)>(execution_tick_function));
}

template <typename ObjectT,
          auto TickFcn,
          auto InitFcn = nullptr,
          auto TeardownFcn = nullptr,
          typename... CtorArgs>
  requires ValidObjectWrapperNode<ObjectT,
                                  TickFcn,
                                  AsyncSimpleNodeReturnTypeInterpreter,
                                  InitFcn,
                                  TeardownFcn>
void NodeRegistry::registerSimpleNodeTypeAsync(const NodeTypeId& node_type_id,
                                               CtorArgs&&... constructor_args) {
  registerNodeType<BehaviorFlowNodeObjectWrapper<
      ObjectT, TickFcn, AsyncSimpleNodeReturnTypeInterpreter, InitFcn, TeardownFcn>>(
      node_type_id, AsyncSimpleNodeReturnTypeInterpreter::validResultIds(),
      std::forward<CtorArgs>(constructor_args)...);
}

inline void NodeRegistry::registerDecisionNodeType(const NodeTypeId& node_type_id,
                                                   BoolNodeFunction auto&& execution_function) {
  registerNodeType<bool, BoolReturnTypeInterpreter>(
      node_type_id, std::forward<decltype(execution_function)>(execution_function));
}

template <typename ObjectT,
          auto ExecutionFcn,
          auto InitFcn = nullptr,
          auto TeardownFcn = nullptr,
          typename... CtorArgs>
  requires ValidObjectWrapperNode<ObjectT, ExecutionFcn, BoolReturnTypeInterpreter, InitFcn, TeardownFcn>
void NodeRegistry::registerDecisionNodeType(const NodeTypeId& node_type_id,
                                            CtorArgs&&... constructor_args) {
  registerNodeType<BehaviorFlowNodeObjectWrapper<ObjectT, ExecutionFcn, BoolReturnTypeInterpreter,
                                                 InitFcn, TeardownFcn>>(
      node_type_id, BoolReturnTypeInterpreter::validResultIds(),
      std::forward<CtorArgs>(constructor_args)...);
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