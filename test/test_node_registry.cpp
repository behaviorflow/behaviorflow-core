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

TEST_F(NodeRegistryTest, RegisterStatefulDecisionNode) {
  class ExecutionCountChecker {
    int counter_{0};
    int threshold_;

   public:
    explicit ExecutionCountChecker(int threshold) : threshold_(threshold) {}
    bool hasExecutedThreshold() {
      ++counter_;
      return counter_ > threshold_;
    }
  };
  const NodeTypeId TypeId{"ExecutionCountChecker"};
  registry.registerDecisionNodeType<ExecutionCountChecker,
                                    &ExecutionCountChecker::hasExecutedThreshold>(TypeId, 3);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeFalseResultId));
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeTrueResultId));
  EXPECT_EQ(node->execute(), ReturnType(DecisionNodeTrueResultId));
}

TEST_F(NodeRegistryTest, RegisterAsyncSimpleNode) {
  class Counter {
   public:
    explicit Counter(int expiration_count) : expiration_count_(expiration_count), count_(0) {};
    void increment() { count_++; }
    bool isExpired() const { return count_ >= expiration_count_; }
    int expiration_count_;
    int count_;
  };
  Counter counter(3);
  const NodeTypeId TypeId{"AsyncSimpleNode"};
  registry.registerSimpleNodeTypeAsync(TypeId, [&counter]() -> AsyncSimpleNodeResult {
    counter.increment();
    if (counter.isExpired()) {
      return AsyncSimpleNodeResult::Finished;
    } else {
      return AsyncSimpleNodeResult::Running;
    }
  });
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
}

TEST_F(NodeRegistryTest, RegisterAsyncSimpleStatefulNodeWithInitFcn) {
  class RunXTimesNode {
    int counter_{0};
    int threshold_{3};

   public:
    explicit RunXTimesNode(int threshold) : threshold_(threshold) {}
    void reset() { counter_ = 0; }
    AsyncSimpleNodeResult execute() {
      ++counter_;
      if (counter_ < threshold_) {
        return AsyncSimpleNodeResult::Running;
      } else {
        return AsyncSimpleNodeResult::Finished;
      }
    }
  };

  const NodeTypeId TypeId{"RunXTimes"};
  registry
      .registerSimpleNodeTypeAsync<RunXTimesNode, &RunXTimesNode::execute, &RunXTimesNode::reset>(
          TypeId, 3);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
}

TEST_F(NodeRegistryTest, RegisterAsyncSimpleStatefulNodeNoInitFcn) {
  class RunXTimesNode {
    int counter_{0};
    int threshold_;

   public:
    explicit RunXTimesNode(int threshold) : threshold_(threshold) {}
    AsyncSimpleNodeResult execute() {
      ++counter_;
      if (counter_ < threshold_) {
        return AsyncSimpleNodeResult::Running;
      } else {
        return AsyncSimpleNodeResult::Finished;
      }
    }
  };

  const NodeTypeId TypeId{"RunXTimes"};
  registry.registerSimpleNodeTypeAsync<RunXTimesNode, &RunXTimesNode::execute>(TypeId, 3);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType::StillRunning());
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
  EXPECT_EQ(node->execute(), ReturnType(SimpleNodeResultId));
}

TEST_F(NodeRegistryTest, RegisterNodeFromNodeClass) {
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

TEST_F(NodeRegistryTest, RegisterCustomNodeType) {
  struct IntTestNodeTypeInterpreter {
    using ResultType = int;
    static std::vector<ResultId> validResultIds() {
      return {ResultId("1"), ResultId("2"), ResultId("3+")};
    }
    static ReturnType toNodeReturnType(int value) {
      if (value >= 3) {
        return ReturnType("3+");
      }
      return ReturnType(std::to_string(value));
    }
  };

  class ExecutionCountIndicator {
    int counter_ = 0;

   public:
    int getExecutionCount() { return ++counter_; }
  };

  const NodeTypeId TypeId{"ExecutionCount"};
  registry.registerNodeType<ExecutionCountIndicator, &ExecutionCountIndicator::getExecutionCount,
                            IntTestNodeTypeInterpreter>(TypeId);
  std::unique_ptr<BehaviorFlowNodeBase> node =
      NodeRegistryAccessor::instantiateNode(registry, TypeId);
  EXPECT_EQ(node->execute(), ReturnType("1"));
  EXPECT_EQ(node->execute(), ReturnType("2"));
  EXPECT_EQ(node->execute(), ReturnType("3+"));
  EXPECT_EQ(node->execute(), ReturnType("3+"));
};