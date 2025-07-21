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
	}

	bool flag = false;
	NodeRegistrar registrar;
};

TEST_F(NodeRegistrarTest, RegisterSimpleNodeWithLambda) {
	const std::string NodeTypeId = "registerSimpleNode";
	const std::string NodeInstanceId = "SimpleNode";
	auto flag = std::make_shared<bool>(false);
	registrar.registerSimpleNodeType(
		NodeTypeId,
		[flag](){*flag = true;}
	);
	NodeWithMetadata node = NodeRegistrarInstantiator::instantiateNode(registrar, NodeTypeId, NodeInstanceId);
	node.node_instance->execute();
	EXPECT_TRUE(*flag);
	EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
	EXPECT_EQ(node.metadata.node_instance_id, NodeInstanceId);
}

TEST_F(NodeRegistrarTest, RegisterSimpleNodeWithBind) {
	setFlagFalse();
	const std::string NodeTypeId = "registerSimpleNodeWithBind";
	const std::string NodeInstanceId = "SimpleNodeWithBind";
	registrar.registerSimpleNodeType(
		NodeTypeId,
		std::bind(
			&NodeRegistrarTest::setFlagTrue,
			this
		)
	);
	NodeWithMetadata node = NodeRegistrarInstantiator::instantiateNode(registrar, NodeTypeId, NodeInstanceId);
	node.node_instance->execute();
	EXPECT_TRUE(flag);
	EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
	EXPECT_EQ(node.metadata.node_instance_id, NodeInstanceId);
}

TEST_F(NodeRegistrarTest, RegisterSimpleNodeWithFunctionPointer) {
	global_flag = false;
	const std::string NodeTypeId = "registerSimpleNodeWithFunctionPointer";
	const std::string NodeInstanceId = "SimpleNodeWithFunctionPointer";
	registrar.registerSimpleNodeType(
		NodeTypeId,
		setGlobalFlagTrue
	);
	NodeWithMetadata node = NodeRegistrarInstantiator::instantiateNode(registrar, NodeTypeId, NodeInstanceId);
	node.node_instance->execute();
	EXPECT_TRUE(global_flag);
	EXPECT_EQ(node.metadata.node_type_id, NodeTypeId);
	EXPECT_EQ(node.metadata.node_instance_id, NodeInstanceId);
}