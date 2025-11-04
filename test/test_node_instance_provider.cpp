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
  const NodeWithMetadata& instance1 = provider->getNodeInstance("node1", "DummyNode");
  const NodeWithMetadata& instance2 = provider->getNodeInstance("node1", "DummyNode");
  EXPECT_EQ(&instance1, &instance2);
  EXPECT_EQ(instance1.metadata.node_type_id, "DummyNode");
  EXPECT_EQ(instance1.metadata.node_instance_id, "node1");
  EXPECT_NE(instance1.node_instance, nullptr);
}

TEST_F(NodeInstanceProviderTest, DifferentNodeIdsReturnDifferentInstances) {
  const NodeWithMetadata& instance1 = provider->getNodeInstance("node1", "DummyNode");
  const NodeWithMetadata& instance2 = provider->getNodeInstance("node2", "DummyNode");
  EXPECT_NE(&instance1, &instance2);
  EXPECT_EQ(instance1.metadata.node_type_id, "DummyNode");
  EXPECT_EQ(instance2.metadata.node_type_id, "DummyNode");
  EXPECT_EQ(instance1.metadata.node_instance_id, "node1");
  EXPECT_EQ(instance2.metadata.node_instance_id, "node2");
  EXPECT_NE(instance1.node_instance, nullptr);
  EXPECT_NE(instance2.node_instance, nullptr);
  EXPECT_NE(instance1.node_instance, instance2.node_instance);
}

TEST_F(NodeInstanceProviderTest, DifferentNodeTypesReturnDifferentInstances) {
  const NodeWithMetadata& instance1 = provider->getNodeInstance("node1", "DummyNode");
  const NodeWithMetadata& instance2 = provider->getNodeInstance("node2", "SetFlagTrue");
  EXPECT_NE(&instance1, &instance2);
  EXPECT_EQ(instance1.metadata.node_type_id, "DummyNode");
  EXPECT_EQ(instance2.metadata.node_type_id, "SetFlagTrue");
  EXPECT_EQ(instance1.metadata.node_instance_id, "node1");
  EXPECT_EQ(instance2.metadata.node_instance_id, "node2");
  instance1.node_instance->execute();
  EXPECT_FALSE(flag_);
  instance2.node_instance->execute();
  EXPECT_TRUE(flag_);
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
