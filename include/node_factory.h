// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__NODE_FACTORY_H_
#define BEHAVIOR_FLOW__NODE_FACTORY_H_

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "behavior_flow_node.h"
#include "utils/behavior_flow_types.h"
#include "utils/concepts.h"

namespace bflow {

class NodeFactory {
 public:
  using NodeConstructor = std::function<std::unique_ptr<BehaviorFlowNodeBase>()>;

  NodeFactory() = default;
  // todo: copy/move constructors?

  template <BehaviorFlowNodeType T, typename... Args>
  void registerNodeType(const NodeTypeId& node_type_id, Args&&... node_constructor_args) {
    if (node_type_constr_map_.find(node_type_id) != node_type_constr_map_.end()) {
      throw std::runtime_error("A node type with id '" + node_type_id + "' is already registered.");
    }

    node_type_constr_map_[node_type_id] = [... args = std::forward<Args>(node_constructor_args)]() {
      return std::make_unique<T>(args...);
    };
  }

  std::unique_ptr<BehaviorFlowNodeBase> createNodeInstance(const NodeTypeId& node_type_id);

 private:
  std::unordered_map<NodeTypeId, NodeConstructor> node_type_constr_map_;
};

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__NODE_FACTORY_H_