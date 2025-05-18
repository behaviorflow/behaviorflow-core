// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "behavior_flow_engine.h"

using namespace bflow;

class BehaviorFlowEngineTest : public ::testing::Test {
 protected:
  BehaviorFlowEngine bf_engine;
  void SetUp() override { bf_engine = BehaviorFlowEngine(); }
};

TEST_F(BehaviorFlowEngineTest, RegisterSimpleConditionNode) {
  // bf_engine.registerNodeType<ConditionNode>(
  // 	"Test Node",
  // 	[]() -> bool { return true; },

  // );
}