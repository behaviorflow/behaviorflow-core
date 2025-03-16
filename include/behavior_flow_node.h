// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_

#include <string>
#include <vector>
#include <stdexcept>

namespace behaviorflow {

class BehaviorFlowNodeBase {
public:
  BehaviorFlowNodeBase() = default;
  virtual ~BehaviorFlowNodeBase() = default;

  void init(std::string node_instance_name);
  std::string run();

private:
  virtual void onInit() = 0;
  virtual std::string runNode() = 0; 
  virtual std::vector<std::string> getValidResultNames() = 0;

  bool initialized_ = false;
  std::string node_instance_name_;
  // std::string node_description_;
};

class SimpleBehaviorFlowNode : public BehaviorFlowNodeBase {
private:
  std::string runNode() override;
  std::vector<std::string> getValidResultNames() override;
  virtual void execute() = 0;
};

// REVIEW: is this class template really necessary?
// Or does it just add unneccesary complexity?
// Consider removing this layer
template <typename ResultT>
class BehaviorFlowNode : public BehaviorFlowNodeBase {
private:
  std::string runNode() override {
    ResultT result = execute();
    return resultTypeToString(result);
  }
  virtual ResultT execute() = 0;
  virtual std::string resultTypeToString(ResultT result_type) = 0;
};

class ConditionNode : public BehaviorFlowNode<bool> {
private:
  std::string resultTypeToString(bool return_type) override;
  std::vector<std::string> getValidResultNames() override;
};

enum class TaskResult { Success, Failure };

std::string toString(TaskResult task_result);

class TaskNode : public BehaviorFlowNode<TaskResult> {
private:
  std::string resultTypeToString(TaskResult result_type) override;
  std::vector<std::string> getValidResultNames() override;
};

// class TaskResult {
//  public:
//   TaskResult(TaskResultType type, const std::string& error_code = "", const std::string& error_message = "")
//       : type_(type), error_code_(error_code), error_message_(error_message) {}

//   std::string toString() const {
//     switch (type_) {
//       case TaskResultType::Success:
//         return "Success";
//       case TaskResultType::Failure:
//         return "Failure";
//       case TaskResultType::Canceled:
//         return "Canceled";
//     }
//     return "";
//   }

//  private:
//   TaskResultType type_;
//   std::string error_code_;
//   std::string error_message_;
// };

}  // end namespace behaviorflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_