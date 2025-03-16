#include "node_factory.h"

namespace behaviorflow {

std::unique_ptr<BehaviorFlowNodeBase> NodeFactory::createNodeInstance(std::string node_instance_name,
                                                                  std::string node_type_name) {
  auto it = node_type_constr_map_.find(node_type_name);
  if (it != node_type_constr_map_.end()) {
    return it->second(node_instance_name, node_type_name);
  }
  return nullptr;
}

}  // namespace behaviorflow