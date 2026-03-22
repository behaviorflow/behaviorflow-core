// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "behavior_flow_engine.h"
#include "node_graph.h"
#include "node_registry.h"
#include "standard_node_library.h"
#include "testing_utils.h"
#include "utils/behavior_flow_types.h"

using namespace bflow;
using namespace bflow::test;
using namespace std::chrono_literals;

class BehaviorFlowEngineTest : public ::testing::Test {
 protected:
  const NodeTypeId IncrementCounterNodeTypeId{"Increment Counter"};
  const NodeTypeId ToggleFlagNodeTypeId{"Toggle Flag"};
  const NodeId TestNodeId{"test_node"};
  int counter_ = 0;
  bool flag_ = false;
  NodeRegistry registry;

  void SetUp() override {
    registry.registerSimpleNodeType(IncrementCounterNodeTypeId, [this]() -> void { counter_++; });
    registry.registerSimpleNodeType(ToggleFlagNodeTypeId, [this]() -> void { flag_ = !flag_; });
  }

  std::vector<NodeGraph::NodeTypeDescription> node_type_descriptions_ = {
      {IncrementCounterNodeTypeId, {SimpleNodeResultId}},
      {ToggleFlagNodeTypeId, {SimpleNodeResultId}},
      {SuccessNodeTypeId, {}},
      {FailureNodeTypeId, {}}};
};

TEST_F(BehaviorFlowEngineTest, ExecuteSimpleGraph) {
  NodeGraph graph = NodeGraph(
      node_type_descriptions_,
      {{NodeId("node1"), IncrementCounterNodeTypeId, {{SimpleNodeResultId, NodeId("node2")}}},
       {NodeId("node2"), IncrementCounterNodeTypeId, {{SimpleNodeResultId, NodeId("node3")}}},
       {NodeId("node3"), ToggleFlagNodeTypeId, {{SimpleNodeResultId, NodeId("end_success")}}},
       {NodeId("end_success"), SuccessNodeTypeId, {}}},
      NodeId("node1"));
  BehaviorFlowEngine bf_engine = BehaviorFlowEngine(std::move(registry), std::move(graph));
  BehaviorFlowResult result = bf_engine.execute(0ms);
  EXPECT_EQ(result, BehaviorFlowResult::Success);
  EXPECT_EQ(counter_, 2);
  EXPECT_TRUE(flag_);
}

TEST_F(BehaviorFlowEngineTest, TickSimpleGraph) {
  NodeGraph graph = NodeGraph(
      node_type_descriptions_,
      {{NodeId("node1"), IncrementCounterNodeTypeId, {{SimpleNodeResultId, NodeId("node2")}}},
       {NodeId("node2"), IncrementCounterNodeTypeId, {{SimpleNodeResultId, NodeId("node3")}}},
       {NodeId("node3"), ToggleFlagNodeTypeId, {{SimpleNodeResultId, NodeId("end_success")}}},
       {NodeId("end_success"), SuccessNodeTypeId, {}}},
      NodeId("node1"));
  BehaviorFlowEngine bf_engine = BehaviorFlowEngine(std::move(registry), std::move(graph));

  BehaviorFlowResult result = bf_engine.tick();
  EXPECT_EQ(result, BehaviorFlowResult::Running);
  EXPECT_EQ(counter_, 1);
  EXPECT_FALSE(flag_);

  result = bf_engine.tick();
  EXPECT_EQ(result, BehaviorFlowResult::Running);
  EXPECT_EQ(counter_, 2);
  EXPECT_FALSE(flag_);

  result = bf_engine.tick();
  EXPECT_EQ(result, BehaviorFlowResult::Running);
  EXPECT_EQ(counter_, 2);
  EXPECT_TRUE(flag_);

  result = bf_engine.tick();
  EXPECT_EQ(result, BehaviorFlowResult::Success);
  EXPECT_EQ(counter_, 2);
  EXPECT_TRUE(flag_);
}

// Todo: Tick graph with multi-cycle node

TEST_F(BehaviorFlowEngineTest, ExecuteGraphWithFailure) {
  NodeGraph graph = NodeGraph(node_type_descriptions_,
                              {{NodeId("node1"),
                                IncrementCounterNodeTypeId,
                                {{SimpleNodeResultId, NodeId("failure_node")}}},
                               {NodeId("failure_node"), FailureNodeTypeId, {}}},
                              NodeId("node1"));
  BehaviorFlowEngine bf_engine = BehaviorFlowEngine(std::move(registry), std::move(graph));
  BehaviorFlowResult result = bf_engine.execute(0ms);
  EXPECT_EQ(result, BehaviorFlowResult::Failure);
  EXPECT_EQ(counter_, 1);
}

TEST_F(BehaviorFlowEngineTest, UnregisteredNodeTypeThrows) {
  NodeGraph graph = NodeGraph(
      {{NodeTypeId("UnregisteredNodeType"), {SimpleNodeResultId}}, {SuccessNodeTypeId, {}}},
      {{NodeId("node1"),
        NodeTypeId("UnregisteredNodeType"),
        {{SimpleNodeResultId, NodeId("end_success")}}},
       {NodeId("end_success"), SuccessNodeTypeId, {}}},
      NodeId("node1"));
  EXPECT_ANY_THROW(BehaviorFlowEngine(std::move(registry), std::move(graph)));
}

TEST_F(BehaviorFlowEngineTest, ResultIdMismatchBetweenGraphAndRegisteredThrows) {
  NodeGraph graph = NodeGraph(
      {{IncrementCounterNodeTypeId, {ResultId("InvalidResultType")}}, {SuccessNodeTypeId, {}}},
      {{NodeId("node1"),
        IncrementCounterNodeTypeId,
        {{ResultId("InvalidResultType"), NodeId("success_node")}}},
       {NodeId("success_node"), SuccessNodeTypeId, {}}},
      NodeId("node1"));
  EXPECT_ANY_THROW(BehaviorFlowEngine(std::move(registry), std::move(graph)));
}

TEST_F(BehaviorFlowEngineTest, ExecuteGraphFromFile) {
  std::string graph_json = R"json(
{
  "node_types": [
    {
      "node_type_id": "Increment Counter",
      "result_ids": [""]
    },
    {
      "node_type_id": "Toggle Flag",
      "result_ids": [""]
    },
    {
      "node_type_id": "Success",
      "result_ids": []
    }
  ],
  "nodes": [
    {
      "node_id": "node1",
      "node_type": "Increment Counter",
      "transitions": {
        "": "node2"
      }
    },
    {
      "node_id": "node2",
      "node_type": "Toggle Flag",
      "transitions": {
        "": "end_success"
      }
    },
    {
      "node_id": "end_success",
      "node_type": "Success",
      "transitions": {}
    }
  ],
  "start_node_id": "node1"
})json";
  ScopedTempFile temp_file(graph_json, ".json");
  BehaviorFlowEngine bf_engine = BehaviorFlowEngine(std::move(registry), temp_file.path());
  BehaviorFlowResult result = bf_engine.execute(0ms);
  EXPECT_EQ(result, BehaviorFlowResult::Success);
  EXPECT_EQ(counter_, 1);
  EXPECT_TRUE(flag_);
}

// Result type mismatch