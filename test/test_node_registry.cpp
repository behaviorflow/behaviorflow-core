// Copyright (c) 2025, Mitch Adams

#include <gtest/gtest.h>

#include <functional>
#include <string>

#include "node_registry.h"
#include "utils/behavior_flow_types.h"

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
  auto local_flag = std::make_shared<bool>(false);
  registry.registerSimpleNodeType(NodeTypeId, [local_flag]() { *local_flag = true; });
  NodeWithMetadata node =
      NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId);
  node.node_instance->execute();
  EXPECT_TRUE(*local_flag);
  EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithBind) {
  setFlagFalse();
  const std::string NodeTypeId = "registerSimpleNodeWithBind";
  registry.registerSimpleNodeType(NodeTypeId, std::bind(&NodeRegistryTest::setFlagTrue, this));
  NodeWithMetadata node =
      NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId);
  node.node_instance->execute();
  EXPECT_TRUE(member_flag_);
  EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithFunctionPointer) {
  global_flag = false;
  const std::string NodeTypeId = "registerSimpleNodeWithFunctionPointer";
  registry.registerSimpleNodeType(NodeTypeId, setGlobalFlagTrue);
  NodeWithMetadata node =
      NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId);
  node.node_instance->execute();
  EXPECT_TRUE(global_flag);
  EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
}

// TEST_F(NodeRegistryTest, RegisterSimpleNodeWithFunctorType) {
//   int global_counter = 0;
//   struct Functor {
//     void operator()() {
//       local_counter_++;
//       global_counter += local_counter_;
//     }
//     int local_counter_ = 0;
//   };
//   const std::string NodeTypeId = "FunctorNode";
//   registry.registerSimpleNodeType<Functor>(NodeTypeId);
//   NodeWithMetadata node1 =
//       NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId);
//   NodeWithMetadata node2 =
//       NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId);
//   node1.node_instance->execute();
//   EXPECT_EQ(node1.metadata.node_type_id, NodeTypeId);
//   EXPECT_EQ(global_counter, 1);
//   node2.node_instance->execute();
//   EXPECT_EQ(global_counter, 3);  // 1 + 2 from two different instances
//   node2.node_instance->execute();
//   EXPECT_EQ(global_counter, 4); // Node 2 should have its own state
// }

// TEST_F(NodeRegistryTest, RegisterSimpleNodeWithFunctorInstance) {
//   struct Functor {
//     void operator()() {
//       counter_++;
//     }
//     int counter_ = 0;
//   };
//   Functor functor;
//   const std::string NodeTypeId = "FunctorNode";
//   registry.registerSimpleNodeType(NodeTypeId, functor);
//   NodeWithMetadata node =
//       NodeRegistryInstantiator::instantiateNode(registry, NodeTypeId);
//   node.node_instance->execute();
//   EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
//   EXPECT_EQ(functor.counter_, 1);
// }

TEST_F(NodeRegistryTest, RegisterDuplicateNodeTypeThrows) {
  const std::string NodeTypeId = "duplicateNodeType";
  registry.registerSimpleNodeType(NodeTypeId, []() {});
  EXPECT_ANY_THROW(registry.registerSimpleNodeType(NodeTypeId, []() {}));
}

TEST_F(NodeRegistryTest, InstantiateUnregisteredNodeTypeThrows) {
  const std::string UnregisteredNodeTypeId = "unregisteredNodeType";
  EXPECT_ANY_THROW(
      NodeRegistryInstantiator::instantiateNode(registry, UnregisteredNodeTypeId));
}