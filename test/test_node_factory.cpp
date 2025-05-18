// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include <memory>
#include <typeinfo>

#include "node_factory.h"
#include "nodes_for_testing.h"

using namespace bflow;

class NodeFactoryTest : public ::testing::Test {
 protected:
  NodeFactory factory;
  void SetUp() override { factory = NodeFactory::getInstance(); }
};

TEST_F(NodeFactoryTest, RegisterNodeClass) {
  factory.registerNodeType<TestNodeSimple>("TestNode");
  auto node = factory.createNodeInstance("TestNodeInstance", "TestNode");
  EXPECT_NE(node, nullptr);
  EXPECT_EQ(typeid(*node), typeid(TestNodeSimple));
  EXPECT_EQ(node->getTypeName(), "TestNode");
  EXPECT_EQ(node->getInstanceName(), "TestNodeInstance");
}

TEST_F(NodeFactoryTest, RegisterNodeClassWithParams) {
  factory.registerNodeType<TestNodeWithConstructorParams>("TestNodeWithParams", 5, "hello");
  auto node = factory.createNodeInstance("TestNodeWithParamsInstance", "TestNodeWithParams");
  EXPECT_NE(node, nullptr);
  EXPECT_EQ(typeid(*node), typeid(TestNodeWithConstructorParams));
  auto testNode = static_cast<TestNodeWithConstructorParams*>(node.get());
  EXPECT_EQ(testNode->getParam1(), 5);
  EXPECT_EQ(testNode->getParam2(), "hello");
}

TEST_F(NodeFactoryTest, SameClassDifferentParamsDifferentTypes) {
  factory.registerNodeType<TestNodeWithConstructorParams>("TestNodeWithParams1", 5, "hello");
  factory.registerNodeType<TestNodeWithConstructorParams>("TestNodeWithParams2", 10, "world");
  auto node1 = factory.createNodeInstance("TestNodeWithParamsInstance1", "TestNodeWithParams1");
  auto node2 = factory.createNodeInstance("TestNodeWithParamsInstance2", "TestNodeWithParams2");
  EXPECT_NE(node1, nullptr);
  EXPECT_NE(node2, nullptr);
  EXPECT_EQ(typeid(*node1), typeid(TestNodeWithConstructorParams));
  EXPECT_EQ(typeid(*node2), typeid(TestNodeWithConstructorParams));
  auto testNode1 = static_cast<TestNodeWithConstructorParams*>(node1.get());
  auto testNode2 = static_cast<TestNodeWithConstructorParams*>(node2.get());
  EXPECT_EQ(testNode1->getParam1(), 5);
  EXPECT_EQ(testNode1->getParam2(), "hello");
  EXPECT_EQ(testNode2->getParam1(), 10);
  EXPECT_EQ(testNode2->getParam2(), "world");
}

TEST_F(NodeFactoryTest, MultipleNodeInstances) {
  factory.registerNodeType<TestNodeSimple>("TestNode");
  auto node1 = factory.createNodeInstance("TestNodeInstance1", "TestNode");
  auto node2 = factory.createNodeInstance("TestNodeInstance2", "TestNode");
  EXPECT_NE(node1, nullptr);
  EXPECT_NE(node2, nullptr);
  EXPECT_EQ(typeid(*node1), typeid(TestNodeSimple));
  EXPECT_EQ(typeid(*node2), typeid(TestNodeSimple));
}

TEST_F(NodeFactoryTest, UnregisteredNodeTypeName) {
  EXPECT_EQ(factory.createNodeInstance("TestNodeInstance", "TestNode"), nullptr);
}

TEST_F(NodeFactoryTest, RepeatedNodeTypeNameRegistration) {
  factory.registerNodeType<TestNodeSimple>("TestNode");
  EXPECT_THROW(factory.registerNodeType<TestNodeSimple>("TestNode"), std::runtime_error);
}

TEST_F(NodeFactoryTest, RepeatedNodeInstanceName) {
  // todo
  // what do we want to do about this? Maybe this is the graph's responsibility to validate this?
  // Do we even want to create different objects for different "node instances"?
}
