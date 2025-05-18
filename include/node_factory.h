// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__NODE_FACTORY_H_
#define BEHAVIOR_FLOW__NODE_FACTORY_H_

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "behavior_flow_node.h"

namespace bflow {

class NodeFactory {
public:
  using NodeConstructor =
      std::function<std::unique_ptr<BehaviorFlowNodeBase>(std::string)>;

  static NodeFactory& getInstance() {
    static NodeFactory instance;
    return instance;
  }

  NodeFactory(const NodeFactory&) = delete;
  NodeFactory& operator=(const NodeFactory&) = delete;
  NodeFactory(NodeFactory&&) = delete;
  NodeFactory& operator=(NodeFactory&&) = delete;
  ~NodeFactory() = default;

  template <typename T, typename... Args>
  void registerNodeType(std::string node_type_id, Args &&...node_constructor_args) {
    static_assert(std::is_base_of<BehaviorFlowNodeBase, T>::value,
                  "Registered node type classes must derive from BehaviorFlowNodeBase");
                  // todo: use c++20 concepts, and pull ths logic out. Just pass in the name
    // T temp_node = T(std::forward<Args>(node_constructor_args)...);
    // std::string node_type_id = temp_node.getTypeName() + " " + node_type_suffix;
    
    if (node_type_constr_map_.find(node_type_id) != node_type_constr_map_.end()) {
      throw std::runtime_error("A node type with id '" + node_type_id +
                               "' is already registered.");
    }
    node_type_constr_map_[node_type_id] = [=](std::string node_type_id) mutable {
      auto node = std::make_unique<T>(std::forward<Args>(node_constructor_args)...);
      // node->init(node_instance_name);
      return std::move(node);
    };
  }

  std::unique_ptr<BehaviorFlowNodeBase> createNodeInstance(std::string node_type_id);
  void clear();

private:
  NodeFactory() = default;
  std::unordered_map<std::string, NodeConstructor> node_type_constr_map_;
};

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__NODE_FACTORY_H_