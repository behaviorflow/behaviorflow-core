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
  void SetUp() override {}
};

TEST_F(NodeFactoryTest, RegisterNodeClass) {
  factory.registerNodeType<TestNodeBasic>(NodeTypeId("TestNode"));
  auto node = factory.createNodeInstance(NodeTypeId("TestNode"));
  EXPECT_NE(node, nullptr);
  EXPECT_EQ(typeid(*node), typeid(TestNodeBasic));
}

TEST_F(NodeFactoryTest, MultipleNodeInstances) {
  factory.registerNodeType<TestNodeBasic>(NodeTypeId("TestNode"));
  auto node1 = factory.createNodeInstance(NodeTypeId("TestNode"));
  auto node2 = factory.createNodeInstance(NodeTypeId("TestNode"));
  EXPECT_NE(node1, node2);
  EXPECT_EQ(typeid(*node1), typeid(TestNodeBasic));
  EXPECT_EQ(typeid(*node2), typeid(TestNodeBasic));
}

TEST_F(NodeFactoryTest, RegisterNodeClassWithParams) {
  factory.registerNodeType<TestNodeWithConstructorParams>(NodeTypeId("TestNodeWithParams"), 5,
                                                          std::string("hello"));
  auto node = factory.createNodeInstance(NodeTypeId("TestNodeWithParams"));
  EXPECT_EQ(typeid(*node), typeid(TestNodeWithConstructorParams));
  auto testNode = static_cast<TestNodeWithConstructorParams*>(node.get());
  EXPECT_EQ(testNode->getParam1(), 5);
  EXPECT_EQ(testNode->getParam2(), "hello");
}

TEST_F(NodeFactoryTest, RegisterMultipleNodesWithDifferentParams) {
  factory.registerNodeType<TestNodeWithConstructorParams>(NodeTypeId("TestNodeWithParams1"), 5,
                                                          "hello");
  factory.registerNodeType<TestNodeWithConstructorParams>(NodeTypeId("TestNodeWithParams2"), 10,
                                                          "world");
  auto node1 = factory.createNodeInstance(NodeTypeId("TestNodeWithParams1"));
  auto node2 = factory.createNodeInstance(NodeTypeId("TestNodeWithParams2"));
  EXPECT_EQ(typeid(*node1), typeid(TestNodeWithConstructorParams));
  EXPECT_EQ(typeid(*node2), typeid(TestNodeWithConstructorParams));
  auto testNode1 = static_cast<TestNodeWithConstructorParams*>(node1.get());
  auto testNode2 = static_cast<TestNodeWithConstructorParams*>(node2.get());
  EXPECT_EQ(testNode1->getParam1(), 5);
  EXPECT_EQ(testNode1->getParam2(), "hello");
  EXPECT_EQ(testNode2->getParam1(), 10);
  EXPECT_EQ(testNode2->getParam2(), "world");
}

TEST_F(NodeFactoryTest, UnregisteredNodeTypeName) {
  EXPECT_EQ(factory.createNodeInstance(NodeTypeId("TestNode")), nullptr);
}

TEST_F(NodeFactoryTest, RepeatedNodeTypeNameRegistration) {
  factory.registerNodeType<TestNodeBasic>(NodeTypeId("TestNode"));
  EXPECT_THROW(factory.registerNodeType<TestNodeBasic>(NodeTypeId("TestNode")), std::runtime_error);
}
