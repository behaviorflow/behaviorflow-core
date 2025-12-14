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
    registry.registerSimpleNodeType("DummyNode", []() {});
    registry.registerSimpleNodeType("SetFlagTrue", [this]() { flag_ = true; });
    provider = std::make_unique<NodeInstanceProvider>(std::move(registry));
  }

  std::unique_ptr<NodeInstanceProvider> provider;
  bool flag_ = false;
};

TEST_F(NodeInstanceProviderTest, SameNodeIdReturnsSameInstance) {
  BehaviorFlowNodeBase& instance1 = provider->getNodeInstance("node1", "DummyNode");
  BehaviorFlowNodeBase& instance2 = provider->getNodeInstance("node1", "DummyNode");
  EXPECT_EQ(&instance1, &instance2);
}

TEST_F(NodeInstanceProviderTest, DifferentNodeIdsReturnDifferentInstances) {
  BehaviorFlowNodeBase& instance1 = provider->getNodeInstance("node1", "DummyNode");
  BehaviorFlowNodeBase& instance2 = provider->getNodeInstance("node2", "DummyNode");
  EXPECT_NE(&instance1, &instance2);
}

TEST_F(NodeInstanceProviderTest, DifferentNodeTypesReturnDifferentInstances) {
  BehaviorFlowNodeBase& instance1 = provider->getNodeInstance("node1", "DummyNode");
  BehaviorFlowNodeBase& instance2 = provider->getNodeInstance("node2", "SetFlagTrue");
  EXPECT_NE(&instance1, &instance2);
  instance1.execute();
  EXPECT_FALSE(flag_);
  instance2.execute();
  EXPECT_TRUE(flag_);
}

TEST_F(NodeInstanceProviderTest, RetrievesMetadataCorrectly) {
  std::optional<NodeTypeMetadata> dummy_metadata = provider->getNodeTypeMetadata("DummyNode");
  std::optional<NodeTypeMetadata> set_flag_metadata = provider->getNodeTypeMetadata("SetFlagTrue");
  ASSERT_TRUE(dummy_metadata.has_value());
  ASSERT_TRUE(set_flag_metadata.has_value());
  EXPECT_EQ(dummy_metadata->node_type_id, "DummyNode");
  EXPECT_EQ(set_flag_metadata->node_type_id, "SetFlagTrue");
}

TEST_F(NodeInstanceProviderTest, ThrowsOnSameNodeIdDifferentTypes) {
  provider->getNodeInstance("node1", "DummyNode");
  EXPECT_ANY_THROW(provider->getNodeInstance("node1", "SetFlagTrue"));
}

TEST_F(NodeInstanceProviderTest, ThrowsOnInvalidNodeType) {
  EXPECT_ANY_THROW(provider->getNodeInstance("node1", "NonExistentNodeType"));
}

TEST_F(NodeInstanceProviderTest, ThrowsOnTypeMismatch) {
  provider->getNodeInstance("node1", "DummyNode");
  EXPECT_ANY_THROW(provider->getNodeInstance("node1", "SetFlagTrue"));
}
