// Copyright (c) 2025, Mitch Adams

#include <gtest/gtest.h>

#include "node_instance_provider.h"  // Adjust include as needed

using namespace bflow;

class NodeInstanceProviderTest : public ::testing::Test {
 public:
 protected:
  void SetUp() override {
    NodeRegistry registry;
    flag_ = false;
    registry.registerSimpleNodeType(NodeTypeId("DummyNode"), []() {});
    registry.registerSimpleNodeType(NodeTypeId("SetFlagTrue"), [this]() { flag_ = true; });
    provider = std::make_unique<NodeInstanceProvider>(std::move(registry));
  }

  std::unique_ptr<NodeInstanceProvider> provider;
  bool flag_ = false;
};

TEST_F(NodeInstanceProviderTest, SameNodeIdReturnsSameInstance) {
  BehaviorFlowNodeBase& instance1 =
      provider->getNodeInstance(NodeId("node1"), NodeTypeId("DummyNode"));
  BehaviorFlowNodeBase& instance2 =
      provider->getNodeInstance(NodeId("node1"), NodeTypeId("DummyNode"));
  EXPECT_EQ(&instance1, &instance2);
}

TEST_F(NodeInstanceProviderTest, DifferentNodeIdsReturnDifferentInstances) {
  BehaviorFlowNodeBase& instance1 =
      provider->getNodeInstance(NodeId("node1"), NodeTypeId("DummyNode"));
  BehaviorFlowNodeBase& instance2 =
      provider->getNodeInstance(NodeId("node2"), NodeTypeId("DummyNode"));
  EXPECT_NE(&instance1, &instance2);
}

TEST_F(NodeInstanceProviderTest, DifferentNodeTypesReturnDifferentInstances) {
  BehaviorFlowNodeBase& instance1 =
      provider->getNodeInstance(NodeId("node1"), NodeTypeId("DummyNode"));
  BehaviorFlowNodeBase& instance2 =
      provider->getNodeInstance(NodeId("node2"), NodeTypeId("SetFlagTrue"));
  EXPECT_NE(&instance1, &instance2);
  instance1.execute();
  EXPECT_FALSE(flag_);
  instance2.execute();
  EXPECT_TRUE(flag_);
}

TEST_F(NodeInstanceProviderTest, RetrievesMetadataCorrectly) {
  std::optional<NodeTypeMetadata> dummy_metadata =
      provider->getNodeTypeMetadata(NodeTypeId("DummyNode"));
  std::optional<NodeTypeMetadata> set_flag_metadata =
      provider->getNodeTypeMetadata(NodeTypeId("SetFlagTrue"));
  ASSERT_TRUE(dummy_metadata.has_value());
  ASSERT_TRUE(set_flag_metadata.has_value());
  EXPECT_EQ(dummy_metadata->node_type_id, NodeTypeId("DummyNode"));
  EXPECT_EQ(set_flag_metadata->node_type_id, NodeTypeId("SetFlagTrue"));
}

TEST_F(NodeInstanceProviderTest, ThrowsOnSameNodeIdDifferentTypes) {
  provider->getNodeInstance(NodeId("node1"), NodeTypeId("DummyNode"));
  EXPECT_ANY_THROW(provider->getNodeInstance(NodeId("node1"), NodeTypeId("SetFlagTrue")));
}

TEST_F(NodeInstanceProviderTest, ThrowsOnInvalidNodeType) {
  EXPECT_ANY_THROW(provider->getNodeInstance(NodeId("node1"), NodeTypeId("NonExistentNodeType")));
}

TEST_F(NodeInstanceProviderTest, ThrowsOnTypeMismatch) {
  provider->getNodeInstance(NodeId("node1"), NodeTypeId("DummyNode"));
  EXPECT_ANY_THROW(provider->getNodeInstance(NodeId("node1"), NodeTypeId("SetFlagTrue")));
}
