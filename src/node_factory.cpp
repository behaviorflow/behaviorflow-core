// Copyright (c) 2025, Mitch Adams

#include "node_factory.h"

namespace bflow {

std::unique_ptr<BehaviorFlowNodeBase> NodeFactory::createNodeInstance(const NodeTypeId& node_type_id) {
  auto it = node_type_constr_map_.find(node_type_id);
  if (it != node_type_constr_map_.end()) {
    return it->second();
  }
  return nullptr;
}

}  // namespace bflow