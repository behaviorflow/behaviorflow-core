// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "node_graph.h"

using namespace behaviorflow;

class NodeGraphTest : public ::testing::Test {
 protected:
  NodeGraph bf_graph;
  void SetUp() override { bf_graph = NodeGraph(); }

  const NodeGraph::NodeId StartNodeId = "start";
  const NodeGraph::NodeId NextNodeId1 = "next1";
  const NodeGraph::NodeId NextNodeId2 = "next2";
  const NodeGraph::NodeId FailEndNodeId = "fail_end";
  const NodeGraph::NodeId SuccessEndNodeId = "success_end";
  const NodeGraph::NodeTypeId TestTaskNodeType = "task";
  const NodeGraph::NodeTypeId TestConditionNodeType = "condition";
  const NodeGraph::NodeTypeId TerminalNodeType = "terminal";
  const NodeGraph::ResultId SuccessResultId = "Success";
  const NodeGraph::ResultId FailureResultId = "Failure";
  const NodeGraph::ResultId TrueResultId = "True";
  const NodeGraph::ResultId FalseResultId = "False";
  const NodeGraph::NodeDescription StartNode = {StartNodeId,
                                                TestConditionNodeType,
                                                {
                                                    {TrueResultId, NextNodeId1},
                                                    {FalseResultId, FailEndNodeId},
                                                }};
  const NodeGraph::NodeDescription NextNode1 = {NextNodeId1,
                                                TestTaskNodeType,
                                                {
                                                    {SuccessResultId, NextNodeId2},
                                                    {FailureResultId, FailEndNodeId},
                                                }};
  const NodeGraph::NodeDescription NextNode2 = {NextNodeId2,
                                                TestTaskNodeType,
                                                {
                                                    {SuccessResultId, SuccessEndNodeId},
                                                    {FailureResultId, FailEndNodeId},
                                                }};
  const NodeGraph::NodeDescription FailEndNode = {FailEndNodeId, TerminalNodeType, {}};
  const NodeGraph::NodeDescription SuccessEndNode = {SuccessEndNodeId, TerminalNodeType, {}};
};

TEST_F(NodeGraphTest, StartNode) {
  bf_graph.addStartNode(StartNode);
  EXPECT_EQ(bf_graph.getStartNode().node_id, StartNode.node_id);
}

TEST_F(NodeGraphTest, SingleTransition) {
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  EXPECT_EQ(bf_graph.getNextNode(StartNode.node_id, TrueResultId).node_id, NextNode1.node_id);
}

TEST_F(NodeGraphTest, MultipleTransitions) {
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(NextNode2);
  bf_graph.addNode(FailEndNode);
  bf_graph.addNode(SuccessEndNode);
  EXPECT_EQ(bf_graph.getNextNode(StartNode.node_id, TrueResultId).node_id, NextNode1.node_id);
  EXPECT_EQ(bf_graph.getNextNode(StartNode.node_id, FalseResultId).node_id, FailEndNode.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode1.node_id, SuccessResultId).node_id, NextNode2.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode1.node_id, FailureResultId).node_id, FailEndNode.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode2.node_id, SuccessResultId).node_id,
            SuccessEndNode.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode2.node_id, FailureResultId).node_id, FailEndNode.node_id);
}

TEST_F(NodeGraphTest, GetFromNonExistentNode) {
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  EXPECT_ANY_THROW(bf_graph.getNextNode("nonexistent", SuccessResultId));
}

TEST_F(NodeGraphTest, NonExistentTransition) {
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  EXPECT_ANY_THROW(bf_graph.getNextNode(StartNode.node_id, "nonexistent"));
}

TEST_F(NodeGraphTest, DestinationNodeNotRegistered) {
  bf_graph.addStartNode(StartNode);
  EXPECT_ANY_THROW(bf_graph.getNextNode(StartNode.node_id, TrueResultId));
}

TEST_F(NodeGraphTest, DuplicateNodeId) {
  bf_graph.addStartNode(StartNode);
  EXPECT_ANY_THROW(bf_graph.addNode(StartNode));
}

TEST_F(NodeGraphTest, CheckAllTransitionsExist) {
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(NextNode2);
  bf_graph.addNode(FailEndNode);
  bf_graph.addNode(SuccessEndNode);
  EXPECT_TRUE(bf_graph.allTransitionedToNodesExist());
}

TEST_F(NodeGraphTest, CheckAllTransitionsExistFail) {
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(NextNode2);
  bf_graph.addNode(FailEndNode);
  EXPECT_FALSE(bf_graph.allTransitionedToNodesExist());
}
