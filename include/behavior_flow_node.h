// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils/behavior_flow_types.h"

namespace bflow {

// class ReturnType {
// public:
//   using ResultId = std::string;
//   ReturnType(ResultId result_id, std::string description)
//       : result_id_(result_id), description_(description) {}
//   ResultId getResultId() const { return result_id_; }
//   std::string getDescription() const { return description_; }
// private:
//   ResultId result_id_;
//   std::string description_;
// }

class BehaviorFlowNodeBase {
 public:
  BehaviorFlowNodeBase() = default;
  virtual ~BehaviorFlowNodeBase() = default;

  // No need to init... static and map
  // void init(const std::string& node_instance_name, );
  // ReturnType run();
  virtual ResultId execute() = 0;

  // virtual NodeTypeAttributes getNodeTypeAttributes() = 0;

  // std::string getInstanceName() const { return node_instance_name_; }
  // virtual std::string getTypeName() const = 0; //todo: make static, use concepts

  // private:
  //   virtual void onInit() = 0;
  //   // virtual ResultId runNode() = 0;
  //   virtual std::string runNode() = 0;
  //   virtual std::vector<std::string> getValidResultIds() = 0;

  //   bool initialized_ = false;
  //   std::string node_instance_name_;
  // std::string node_description_;
};

// <ResultT>
// class BehaviorFlowNode : public BehaviorFlowNodeBase {

// }

class SimpleBehaviorFlowNode : public BehaviorFlowNodeBase {
 public:
  SimpleBehaviorFlowNode() = delete;
  SimpleBehaviorFlowNode(std::function<void()> execution_function)
      : execution_function_(execution_function) {}

  ResultId execute() override {
    if (!execution_function_) {
      throw std::runtime_error("No execution function defined for SimpleBehaviorFlowNode.");
    }
    execution_function_();
    return ResultId("");
  }

 private:
  std::function<void()> execution_function_;
};

// // REVIEW: is this class template really necessary?
// // Or does it just add unneccesary complexity?
// // Consider removing this layer
// template <typename ResultT>
// class BehaviorFlowNode : public BehaviorFlowNodeBase {
// private:
//   std::string runNode() override {
//     ResultT result = execute();
//     return resultTypeToString(result);
//   }
//   virtual ResultT execute() = 0;
//   virtual std::string resultTypeToString(ResultT result_type) = 0;
// };

// class ConditionNode : public BehaviorFlowNode<bool> {
// private:
//   std::string resultTypeToString(bool return_type) override;
//   std::vector<std::string> getValidResultIds() override;
// };

// enum class TaskResult { Success, Failure };

// std::string toString(TaskResult task_result);

// class TaskNode : public BehaviorFlowNode<TaskResult> {
// private:
//   std::string resultTypeToString(TaskResult result_type) override;
//   std::vector<std::string> getValidResultIds() override;
// };

// class TaskResult {
//  public:
//   TaskResult(TaskResultType type, const std::string& error_code = "", const std::string&
//   error_message = "")
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

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_