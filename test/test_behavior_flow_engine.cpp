// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "behavior_flow_engine.h"
#include "node_graph.h"
#include "node_registry.h"
#include "standard_node_library.h"
#include "utils/behavior_flow_types.h"

using namespace bflow;

class BehaviorFlowEngineTest : public ::testing::Test {
 protected:
  const NodeTypeId IncrementCounterNodeTypeId = "Increment Counter";
  const NodeTypeId ToggleFlagNodeTypeId = "Toggle Flag";
  const NodeId TestNodeId = "test_node";
  int counter_ = 0;
  bool flag_ = false;

  BehaviorFlowEngine bf_engine;
  void SetUp() override {
    NodeRegistry registry;
    registry.registerSimpleNodeType(IncrementCounterNodeTypeId, [this]() -> void { counter_++; });
    registry.registerSimpleNodeType(ToggleFlagNodeTypeId, [this]() -> void { flag_ = !flag_; });
    bf_engine = BehaviorFlowEngine(std::move(registry));
  }

  std::vector<NodeGraph::NodeTypeDescription> node_type_descriptions_ = {
      {IncrementCounterNodeTypeId, {""}},
      {ToggleFlagNodeTypeId, {""}},
      {SuccessNodeTypeId, {}},
      {FailureNodeTypeId, {}}};
};

TEST_F(BehaviorFlowEngineTest, ExecuteSimpleGraph) {
  NodeGraph graph = NodeGraph(node_type_descriptions_,
                              {{"node1", IncrementCounterNodeTypeId, {{"", "node2"}}},
                               {"node2", IncrementCounterNodeTypeId, {{"", "node3"}}},
                               {"node3", ToggleFlagNodeTypeId, {{"", "end_success"}}},
                               {"end_success", SuccessNodeTypeId, {}}},
                              "node1");

  BehaviorFlowResult result = bf_engine.execute(graph);
  EXPECT_EQ(result, BehaviorFlowResult::Success);
  EXPECT_EQ(counter_, 2);
  EXPECT_TRUE(flag_);
}

TEST_F(BehaviorFlowEngineTest, ExecuteGraphWithFailure) {
  NodeGraph graph = NodeGraph(node_type_descriptions_,
                              {{"node1", IncrementCounterNodeTypeId, {{"", "failure_node"}}},
                               {"failure_node", FailureNodeTypeId, {}}},
                              "node1");

  BehaviorFlowResult result = bf_engine.execute(graph);
  EXPECT_EQ(result, BehaviorFlowResult::Failure);
  EXPECT_EQ(counter_, 1);
}

TEST_F(BehaviorFlowEngineTest, UnregisteredNodeTypeThrows) {
  NodeGraph graph = NodeGraph({{"UnregisteredNodeType", {""}}, {SuccessNodeTypeId, {}}},
                              {{"node1", "UnregisteredNodeType", {{"", "end_success"}}},
                               {"end_success", SuccessNodeTypeId, {}}},
                              "node1");
  EXPECT_ANY_THROW(bf_engine.execute(graph));
}

TEST_F(BehaviorFlowEngineTest, NodeTypeResultMismatchThrows) {
  NodeGraph graph =
      NodeGraph({{IncrementCounterNodeTypeId, {"InvalidResultType"}}, {SuccessNodeTypeId, {}}},
                {{"node1", IncrementCounterNodeTypeId, {{"InvalidResultType", "success_node"}}},
                 {"success_node", SuccessNodeTypeId, {}}},
                "node1");
  EXPECT_ANY_THROW(bf_engine.execute(graph));
}

// Result type mismatch