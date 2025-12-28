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

class BehaviorFlowEngineTest : public ::testing::Test {
 protected:
  const NodeTypeId IncrementCounterNodeTypeId{"Increment Counter"};
  const NodeTypeId ToggleFlagNodeTypeId{"Toggle Flag"};
  const NodeId TestNodeId{"test_node"};
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
      {IncrementCounterNodeTypeId, {ResultId("")}},
      {ToggleFlagNodeTypeId, {ResultId("")}},
      {SuccessNodeTypeId, {}},
      {FailureNodeTypeId, {}}};
};

TEST_F(BehaviorFlowEngineTest, ExecuteSimpleGraph) {
  NodeGraph graph =
      NodeGraph(node_type_descriptions_,
                {{NodeId("node1"), IncrementCounterNodeTypeId, {{ResultId(""), NodeId("node2")}}},
                 {NodeId("node2"), IncrementCounterNodeTypeId, {{ResultId(""), NodeId("node3")}}},
                 {NodeId("node3"), ToggleFlagNodeTypeId, {{ResultId(""), NodeId("end_success")}}},
                 {NodeId("end_success"), SuccessNodeTypeId, {}}},
                NodeId("node1"));

  BehaviorFlowResult result = bf_engine.execute(graph);
  EXPECT_EQ(result, BehaviorFlowResult::Success);
  EXPECT_EQ(counter_, 2);
  EXPECT_TRUE(flag_);
}

TEST_F(BehaviorFlowEngineTest, ExecuteGraphWithFailure) {
  NodeGraph graph = NodeGraph(
      node_type_descriptions_,
      {{NodeId("node1"), IncrementCounterNodeTypeId, {{ResultId(""), NodeId("failure_node")}}},
       {NodeId("failure_node"), FailureNodeTypeId, {}}},
      NodeId("node1"));

  BehaviorFlowResult result = bf_engine.execute(graph);
  EXPECT_EQ(result, BehaviorFlowResult::Failure);
  EXPECT_EQ(counter_, 1);
}

TEST_F(BehaviorFlowEngineTest, UnregisteredNodeTypeThrows) {
  NodeGraph graph =
      NodeGraph({{NodeTypeId("UnregisteredNodeType"), {ResultId("")}}, {SuccessNodeTypeId, {}}},
                {{NodeId("node1"),
                  NodeTypeId("UnregisteredNodeType"),
                  {{ResultId(""), NodeId("end_success")}}},
                 {NodeId("end_success"), SuccessNodeTypeId, {}}},
                NodeId("node1"));
  EXPECT_ANY_THROW(bf_engine.execute(graph));
}

TEST_F(BehaviorFlowEngineTest, NodeTypeResultMismatchThrows) {
  NodeGraph graph = NodeGraph(
      {{IncrementCounterNodeTypeId, {ResultId("InvalidResultType")}}, {SuccessNodeTypeId, {}}},
      {{NodeId("node1"),
        IncrementCounterNodeTypeId,
        {{ResultId("InvalidResultType"), NodeId("success_node")}}},
       {NodeId("success_node"), SuccessNodeTypeId, {}}},
      NodeId("node1"));
  EXPECT_ANY_THROW(bf_engine.execute(graph));
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
  BehaviorFlowResult result = bf_engine.execute(temp_file.path());
  EXPECT_EQ(result, BehaviorFlowResult::Success);
  EXPECT_EQ(counter_, 1);
  EXPECT_TRUE(flag_);
}

// Result type mismatch