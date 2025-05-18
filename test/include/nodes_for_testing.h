#ifndef BEHAVIOR_FLOW__TEST__TEST_NODES_H_
#define BEHAVIOR_FLOW__TEST__TEST_NODES_H_

#include <iostream>

#include "behavior_flow_node.h"

namespace bflow {

// class TestNodeSimple : public SimpleBehaviorFlowNode {
//  public:
//   TestNodeSimple() = default;
//   std::string getTypeName() const override { return "TestNodeSimple"; }

//  private:
//   void onInit() override {};
//   void execute() override {};
// };

// class TestNodeWithConstructorParams : public SimpleBehaviorFlowNode {
//  public:
//   TestNodeWithConstructorParams() = delete;
//   TestNodeWithConstructorParams(int param1, std::string param2)
//       : param1_(param1), param2_(param2) {}

//   int getParam1() { return param1_; }

//   std::string getParam2() { return param2_; }

//   std::string getTypeName() const override { return "TestNodeWithConstructorParams"; }

//  private:
//   int param1_;
//   std::string param2_;

//  private:
//   void onInit() override {};
//   void execute() override {};
// };

// class AddTwoIntsNode : public SimpleBehaviorFlowNode {
// public:
//   AddTwoIntsNode() = default;

//   void run() override {
//     int a = getInput("Addend 1");
//     int b = getInput("Addend 2");
//     int sum = a + b;
//     setOutput("Sum", sum);
//   }

// private:

//   static NodeTypeAttributes getNodeTypeAttributes() {
//     NodeTypeAttributes attributes;
//     attributes.node_type_id = "Add Two Integers";
//     attributes.node_type_description = "Adds two integers together";
//     attributes.parameters = {
//       InputParameter<int>("Addend 1", "First integer to add"),
//       InputParameter<int>("Addend 2", "Second integer to add"),
//       OutputParameter<int>("Sum", "Sum of the two addends")
//     };
//     return attributes;
//   }
// };

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_NODE_H_
