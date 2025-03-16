#include "behavior_flow_node.h"

namespace behaviorflow {

void BehaviorFlowNodeBase::init(std::string node_instance_name) {
  if (!initialized_) {
    node_instance_name_ = node_instance_name;
    onInit();
    initialized_ = true;
  }
  // else?
}

std::string BehaviorFlowNodeBase::run() {
  if (!initialized_) {
    throw std::runtime_error("Node not initialized");
  }
  return runNode();
}

std::string SimpleBehaviorFlowNode::runNode() { 
  execute();
  return ""; 
}

std::vector<std::string> SimpleBehaviorFlowNode::getValidResultNames() {
  return {""};
}

std::string ConditionNode::resultTypeToString(bool result_type) {
  return result_type ? "True" : "False";
}

std::vector<std::string> ConditionNode::getValidResultNames() {
  return {resultTypeToString(true), resultTypeToString(false)};
}

std::string toString(TaskResult task_result) {
  switch (task_result)
  {
  case TaskResult::Success:
    return "Success";
  case TaskResult::Failure:
    return "Failure"; 
  default:
    return "Unknown";
  }
}

std::string TaskNode::resultTypeToString(TaskResult result_type) { 
  return toString(result_type);
}

std::vector<std::string> TaskNode::getValidResultNames() { 
  return {resultTypeToString(TaskResult::Success), resultTypeToString(TaskResult::Failure)};
}

// static NodeTypeAttributes BehaviorFlowNodeBase::getNodeTypeAttributes() {
//   NodeTypeAttributes attributes;
//   attributes.node_type_name = "Node Name";
//   attributes.node_type_description = "Description of the node type";
//   attributes.parameters = {
//     NodeParameter<std::string>("param1", ParameterType::InputParam, "Param 1 description", "default_value"),
//     NodeParameter<int>("param2", ParameterType::OutputParam, "Param 2 description", 42),
//     NodeParameter<CustomType>("param3", ParameterType::RuntimeParam, "Param 3 description")
//   }
// }


}  // namespace behaviorflow