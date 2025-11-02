// Copyright (c) 2025, Mitch Adams

#include <gtest/gtest.h>

#include <functional>
#include <string>

#include "utils/behavior_flow_types.h"
#include "node_registry.h"

using namespace bflow;

bool global_flag = false;
void setGlobalFlagTrue() { global_flag = true; }

class NodeRegistryTest : public ::testing::Test {
 public:
  void setFlagTrue() { member_flag_ = true; }

  void setFlagFalse() { member_flag_ = false; }

 protected:
  void SetUp() override {}

  bool member_flag_ = false;
  NodeRegistry registry;
};

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithLambda) {
  const std::string NodeTypeId = "registerSimpleNode";
  const std::string NodeInstanceId = "SimpleNode";
  auto local_flag = std::make_shared<bool>(false);
  registry.registerSimpleNodeType(NodeTypeId, [local_flag]() { *local_flag = true; });
  NodeWithMetadata node =
      NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId, NodeInstanceId);
  node.node_instance->execute();
  EXPECT_TRUE(*local_flag);
  EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
  EXPECT_EQ(node.metadata.node_instance_id, NodeInstanceId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithBind) {
  setFlagFalse();
  const std::string NodeTypeId = "registerSimpleNodeWithBind";
  const std::string NodeInstanceId = "SimpleNodeWithBind";
  registry.registerSimpleNodeType(NodeTypeId, std::bind(&NodeRegistryTest::setFlagTrue, this));
  NodeWithMetadata node =
      NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId, NodeInstanceId);
  node.node_instance->execute();
  EXPECT_TRUE(member_flag_);
  EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
  EXPECT_EQ(node.metadata.node_instance_id, NodeInstanceId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithFunctionPointer) {
  global_flag = false;
  const std::string NodeTypeId = "registerSimpleNodeWithFunctionPointer";
  const std::string NodeInstanceId = "SimpleNodeWithFunctionPointer";
  registry.registerSimpleNodeType(NodeTypeId, setGlobalFlagTrue);
  NodeWithMetadata node =
      NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId, NodeInstanceId);
  node.node_instance->execute();
  EXPECT_TRUE(global_flag);
  EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
  EXPECT_EQ(node.metadata.node_instance_id, NodeInstanceId);
}