#ifndef BEHAVIOR_FLOW__TEST__TEST_NODES_H_
#define BEHAVIOR_FLOW__TEST__TEST_NODES_H_

#include <iostream>

#include "behavior_flow_node.h"

namespace behaviorflow {

class TestNodeSimple : public BehaviorFlowNode {
 public:
  TestNodeSimple() = default;
};

class TestNodeWithConstructorParams : public BehaviorFlowNode {
 public:
  TestNodeWithConstructorParams() = delete;
  TestNodeWithConstructorParams(int param1, std::string param2)
      : param1_(param1), param2_(param2) {}

  int getParam1() { return param1_; }

  std::string getParam2() { return param2_; }

 private:
  int param1_;
  std::string param2_;
};

class AddTwoIntsNode : public BehaviorFlowNode {
 public:
  AddTwoIntsNode() = default;

  std::string run() override {
    int a = getInput("Addend 1");
    int b = getInput("Addend 2");
    int sum = a + b;
    setOutput("Sum", sum);
    return "Success";
  }

  static NodeTypeAttributes getNodeTypeAttributes() {
    NodeTypeAttributes attributes;
    attributes.node_type_name = "Add Two Integers";
    attributes.node_type_description = "Adds two integers together";
    attributes.parameters = {
      InputParameter<int>("Addend 1", "First integer to add"),
      InputParameter<int>("Addend 2", "Second integer to add"),
      OutputParameter<int>("Sum", "Sum of the two addends")
    };
    return attributes;
  }
};


}  // end namespace behaviorflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_
