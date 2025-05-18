// Copyright (c) 2025, Mitch Adams

#include <gtest/gtest.h>
#include "node_registrar.h"
#include "node_factory.h"

using namespace bflow;

bool global_flag = false;
void setGlobalFlagTrue() {
	global_flag = true;
}

class NodeRegistrarTest : public ::testing::Test {
 public:
 	void setFlagTrue() {
		flag = true;
	}

	void setFlagFalse() {
		flag = false;
	}

 protected:
	void SetUp() override {
		NodeFactory::getInstance().clear();
	}

	bool flag = false;
};

TEST_F(NodeRegistrarTest, registerSimpleNodeWithLambda) {
	const std::string NodeTypeId = "registerSimpleNode";
	auto flag = std::make_shared<bool>(false);
	registerSimpleNodeType(
		NodeTypeId,
		[flag](){*flag = true;}
	);
	std::unique_ptr<BehaviorFlowNodeBase> node = NodeFactory::getInstance().createNodeInstance(NodeTypeId);
	node->execute();
	EXPECT_TRUE(*flag);
}

TEST_F(NodeRegistrarTest, registerSimpleNodeWithBind) {
	setFlagFalse();
	const std::string NodeTypeId = "registerSimpleNodeWithBind";
	registerSimpleNodeType(
		NodeTypeId,
		std::bind(
			&NodeRegistrarTest::setFlagTrue,
			this
		)
	);
	std::unique_ptr<BehaviorFlowNodeBase> node = NodeFactory::getInstance().createNodeInstance(NodeTypeId);
	node->execute();
	EXPECT_TRUE(flag);
}

TEST_F(NodeRegistrarTest, registerSimpleNodeWithFunctionPointer) {
	global_flag = false;
	const std::string NodeTypeId = "registerSimpleNodeWithFunctionPointer";
	registerSimpleNodeType(
		NodeTypeId,
		setGlobalFlagTrue
	);
	std::unique_ptr<BehaviorFlowNodeBase> node = NodeFactory::getInstance().createNodeInstance(NodeTypeId);
	node->execute();
	EXPECT_TRUE(global_flag);
}