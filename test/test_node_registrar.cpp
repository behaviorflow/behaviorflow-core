// Copyright (c) 2025, Mitch Adams

#include <gtest/gtest.h>
#include "node_registrar.h"
#include "node_factory.h"

using namespace bflow;

TEST(NodeRegistrarTest, registerSimpleNode) {
	auto flag = std::make_shared<bool>(false);
	registerSimpleNodeType(
		"node_type_id",
		[flag](){*flag = true;}
	);
	std::unique_ptr<BehaviorFlowNodeBase> node = NodeFactory::getInstance().createNodeInstance("node_type_id");
	node->execute();
	EXPECT_TRUE(*flag);
}