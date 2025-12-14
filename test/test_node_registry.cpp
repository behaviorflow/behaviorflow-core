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
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, NodeTypeId);
  node->execute();
  EXPECT_TRUE(*local_flag);

  std::optional<NodeTypeMetadata> metadata =
      NodeRegistryAccessor::getNodeTypeMetadata(registry, NodeTypeId);
  ASSERT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata->node_type_id, NodeTypeId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithBind) {
  setFlagFalse();
  const std::string NodeTypeId = "registerSimpleNodeWithBind";
  registry.registerSimpleNodeType(NodeTypeId, std::bind(&NodeRegistryTest::setFlagTrue, this));
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, NodeTypeId);
  node->execute();
  EXPECT_TRUE(member_flag_);

  std::optional<NodeTypeMetadata> metadata =
      NodeRegistryAccessor::getNodeTypeMetadata(registry, NodeTypeId);
  ASSERT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata->node_type_id, NodeTypeId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithFunctionPointer) {
  global_flag = false;
  const std::string NodeTypeId = "registerSimpleNodeWithFunctionPointer";
  registry.registerSimpleNodeType(NodeTypeId, setGlobalFlagTrue);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, NodeTypeId);
  node->execute();
  EXPECT_TRUE(global_flag);

  std::optional<NodeTypeMetadata> metadata =
      NodeRegistryAccessor::getNodeTypeMetadata(registry, NodeTypeId);
  ASSERT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata->node_type_id, NodeTypeId);
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
//   std::unique_ptr<BehaviorFlowNodeBase> node1 =
//       NodeRegistryAccessor::instantiateNode(registry, NodeTypeId);
//   std::unique_ptr<BehaviorFlowNodeBase> node2 =
//       NodeRegistryAccessor::instantiateNode(registry, NodeTypeId);
//   node1->execute();
//   EXPECT_EQ(node1.metadata.node_type_id, NodeTypeId);
//   EXPECT_EQ(global_counter, 1);
//   node2->execute();
//   EXPECT_EQ(global_counter, 3);  // 1 + 2 from two different instances
//   node2->execute();
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
//   std::unique_ptr<BehaviorFlowNodeBase> node =
//       NodeRegistryAccessor::instantiateNode(registry, NodeTypeId);
//   node->execute();
//   EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
//   EXPECT_EQ(functor.counter_, 1);
// }

TEST_F(NodeRegistryTest, RegisterDuplicateNodeTypeThrows) {
  const std::string NodeTypeId = "duplicateNodeType";
  registry.registerSimpleNodeType(NodeTypeId, []() {});
  EXPECT_ANY_THROW(registry.registerSimpleNodeType(NodeTypeId, []() {}));
}

TEST_F(NodeRegistryTest, InstantiateUnregisteredNodeType) {
  const std::string UnregisteredNodeTypeId = "unregisteredNodeType";
  EXPECT_EQ(NodeRegistryAccessor::instantiateNode(registry, UnregisteredNodeTypeId), nullptr);
}