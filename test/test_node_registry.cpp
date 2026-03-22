// Copyright (c) 2025, Mitch Adams

#include <gtest/gtest.h>

#include <functional>
#include <string>

#include "node_registry.h"
#include "utils/behavior_flow_types.h"

using namespace bflow;

namespace {
bool global_flag = false;
void setGlobalFlagTrue() { global_flag = true; }
bool getGlobalFlag() { return global_flag; }
bool getFlagArg(bool arg) { return arg; }
}  // namespace

class NodeRegistryTest : public ::testing::Test {
 public:
  void setFlagTrue() { member_flag_ = true; }

  void setFlagFalse() { member_flag_ = false; }

  bool getFlag() const { return member_flag_; }

 protected:
  void SetUp() override {}

  bool member_flag_ = false;
  NodeRegistry registry;
};

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithLambda) {
  const NodeTypeId TypeId{"registerSimpleNode"};
  auto local_flag = std::make_shared<bool>(false);
  registry.registerSimpleNodeType(TypeId, [local_flag]() { *local_flag = true; });
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_TRUE(*local_flag);

  std::optional<NodeTypeMetadata> metadata =
      NodeRegistryAccessor::getNodeTypeMetadata(registry, TypeId);
  ASSERT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata->node_type_id, TypeId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithBind) {
  setFlagFalse();
  const NodeTypeId TypeId{"registerSimpleNodeWithBind"};
  registry.registerSimpleNodeType(TypeId, std::bind(&NodeRegistryTest::setFlagTrue, this));
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_TRUE(member_flag_);

  std::optional<NodeTypeMetadata> metadata =
      NodeRegistryAccessor::getNodeTypeMetadata(registry, TypeId);
  ASSERT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata->node_type_id, TypeId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithFunctionPointer) {
  global_flag = false;
  const NodeTypeId TypeId{"registerSimpleNodeWithFunctionPointer"};
  registry.registerSimpleNodeType(TypeId, setGlobalFlagTrue);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_TRUE(global_flag);

  std::optional<NodeTypeMetadata> metadata =
      NodeRegistryAccessor::getNodeTypeMetadata(registry, TypeId);
  ASSERT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata->node_type_id, TypeId);
}

TEST_F(NodeRegistryTest, RegisterSimpleNodeWithFunctorReference) {
  struct Functor {
    void operator()() { counter_++; }
    int counter_ = 0;
  };
  Functor functor;
  const NodeTypeId TypeId{"FunctorNode"};
  registry.registerSimpleNodeType(TypeId, std::ref(functor));
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_EQ(functor.counter_, 1);
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_EQ(functor.counter_, 2);
}

TEST_F(NodeRegistryTest, RegisterDuplicateNodeTypeThrows) {
  const NodeTypeId TypeId{"duplicateNodeType"};
  registry.registerSimpleNodeType(TypeId, []() {});
  EXPECT_ANY_THROW(registry.registerSimpleNodeType(TypeId, []() {}));
}

TEST_F(NodeRegistryTest, InstantiateUnregisteredNodeType) {
  const NodeTypeId UnregisteredNodeTypeId{"unregisteredNodeType"};
  EXPECT_EQ(NodeRegistryAccessor::instantiateNode(registry, UnregisteredNodeTypeId), nullptr);
}

TEST_F(NodeRegistryTest, RegisterDecisionNodeWithLambda) {
  const NodeTypeId TypeId{"DecisionNode"};
  auto local_flag = std::make_shared<bool>(false);
  registry.registerDecisionNodeType(TypeId, [local_flag]() { return *local_flag; });
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  *local_flag = true;
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeTrueResultId));
}

TEST_F(NodeRegistryTest, RegisterDecisionNodeWithBind) {
  const NodeTypeId TypeId{"DecisionNodeWithBind"};
  registry.registerDecisionNodeType(TypeId, std::bind(&NodeRegistryTest::getFlag, this));
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  setFlagFalse();
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  setFlagTrue();
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeTrueResultId));
}

TEST_F(NodeRegistryTest, RegisterDecisionNodeWithFunctionPointer) {
  const NodeTypeId TypeId{"DecisionNodeWithFunctionPointer"};
  registry.registerDecisionNodeType(TypeId, getGlobalFlag);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  global_flag = false;
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  global_flag = true;
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeTrueResultId));
}

TEST_F(NodeRegistryTest, RegisterDecisionNodeWithBindWithArgs) {
  const NodeTypeId TypeId{"DecisionNodeWithBindWithArgs"};
  registry.registerDecisionNodeType(TypeId, std::bind(getFlagArg, true));
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeTrueResultId));
}

TEST_F(NodeRegistryTest, RegisterDecisionNodeWithFunctorReference) {
  struct Functor {
    bool operator()() { return ++counter_ > NumExecutions; }
    int counter_ = 0;
    const int NumExecutions = 3;
  };
  Functor functor;
  const NodeTypeId TypeId{"DecisionFunctorNode"};
  registry.registerDecisionNodeType(TypeId, std::ref(functor));
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeTrueResultId));
}

TEST_F(NodeRegistryTest, RegisterRunningDecisionNode) {
  class Counter {
   public:
    explicit Counter(int expiration_count) : expiration_count_(expiration_count), count_(0) {};
    void increment() { count_++; }
    bool isExpired() const { return count_ >= expiration_count_; }
    int expiration_count_;
    int count_;
  };
  Counter counter(3);
  const NodeTypeId TypeId{"RunningDecisionNode"};
  registry.registerSimpleNodeType(TypeId, [&counter]() -> SimpleNodeResult {
    counter.increment();
    if (counter.isExpired()) {
      return SimpleNodeResult::Finished;
    } else {
      return SimpleNodeResult::Running;
    }
  });
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
}

TEST_F(NodeRegistryTest, RegisterNodeFromClass) {
  class TestNode : public BehaviorFlowNodeBase {
   public:
    explicit TestNode(int max_counter) : max_counter_(max_counter) {}
    ReturnType execute() {
      if (counter_ < max_counter_) {
        counter_++;
        return ReturnType("Ok");
      }
      return ReturnType("Max");
    }
    int max_counter_;
    int counter_ = 0;
  };
  const NodeTypeId TypeId{"TestNode"};
  registry.registerNodeType<TestNode>(TypeId, {ResultId("Ok"), ResultId("Max")}, 3);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType("Ok"));
  EXPECT_EQ(node->execute(), ReturnType("Ok"));
  EXPECT_EQ(node->execute(), ReturnType("Ok"));
  EXPECT_EQ(node->execute(), ReturnType("Max"));
}

TEST_F(NodeRegistryTest, RegisterCustomNodeTypeWithFunctorReference) {
  struct Functor {
    int operator()() { return ++counter_; }
    int counter_ = 0;
  };
  struct IntTestNodeTypeInterpreter {
    static std::vector<ResultId> validResultIds() { return {ResultId("1"), ResultId("2")}; }
    static ReturnType toNodeReturnType(int value) { return ReturnType(std::to_string(value)); }
  };
  Functor functor;
  const NodeTypeId TypeId{"FunctorNodeType"};
  registry.registerNodeType<int, IntTestNodeTypeInterpreter>(TypeId, std::ref(functor));
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType("1"));
  EXPECT_EQ(functor.counter_, 1);
  EXPECT_EQ(node->execute(), ReturnType("2"));
  EXPECT_EQ(functor.counter_, 2);
  std::optional<NodeTypeMetadata> metadata =
      NodeRegistryAccessor::getNodeTypeMetadata(registry, TypeId);
  ASSERT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata->node_type_id, TypeId);
  EXPECT_EQ(metadata->valid_result_ids, IntTestNodeTypeInterpreter::validResultIds());
}