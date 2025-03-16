// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "behavior_flow_graph.h"
#include "nodes_for_testing.h"

using namespace behaviorflow;

class BehaviorFlowGraphTest : public ::testing::Test {
 protected:
  BehaviorFlowGraph bf_graph;
  void SetUp() override { bf_graph = BehaviorFlowGraph(); }
};

TEST_F(BehaviorFlowGraphTest, EmptyGraph) {}

TEST_F(BehaviorFlowGraphTest, SingleNode) {
  bf_graph.add_node<TesrNodeSimple>("N")
}