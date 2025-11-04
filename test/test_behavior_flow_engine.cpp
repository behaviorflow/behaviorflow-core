// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "behavior_flow_engine.h"
#include "node_graph.h"
#include "node_registry.h"
#include "utils/behavior_flow_types.h"
#include "utils/behavior_flow_utils.h"

using namespace bflow;

class BehaviorFlowEngineTest : public ::testing::Test {
 protected:
  const NodeTypeId SetFlag1NodeTypeId = "Set Flag 1";
  const NodeTypeId SetFlag2NodeTypeId = "Set Flag 2";
  const NodeId TestNodeId = "test_node";
  bool flag1 = false;
  bool flag2 = false;

  BehaviorFlowEngine bf_engine;
  void SetUp() override {
    NodeRegistry registry;
    registry.registerSimpleNodeType(SetFlag1NodeTypeId, [this]() -> void {
      flag1 = true;
    });  // todo: seems like this is in another test, should make it a shared utility
    registry.registerSimpleNodeType(SetFlag2NodeTypeId, [this]() -> void { flag2 = true; });

    bf_engine = BehaviorFlowEngine(std::move(registry));
  }

  std::vector<NodeGraph::NodeTypeDescription> node_type_descriptions_ = {{SetFlag1NodeTypeId, {""}},
                                                                         {SetFlag2NodeTypeId, {""}},
                                                                         {SuccessNodeTypeId, {}},
                                                                         {FailureNodeTypeId, {}}};
};

TEST_F(BehaviorFlowEngineTest, ExecuteSimpleGraph) {
  NodeGraph graph = NodeGraph(node_type_descriptions_,
                              {{"node1", SetFlag1NodeTypeId, {{"", "node2"}}},
                               {"node2", SetFlag2NodeTypeId, {{"", "end_success"}}},
                               {"end_success", SuccessNodeTypeId, {}}},
                              "node1");

  BehaviorFlowResult result = bf_engine.execute(graph);
  EXPECT_EQ(result, BehaviorFlowResult::Success);
  EXPECT_TRUE(flag1);
  EXPECT_TRUE(flag2);
}