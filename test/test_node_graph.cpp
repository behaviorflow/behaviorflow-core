// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "node_graph.h"
#include "utils/behavior_flow_types.h"
#include "utils/behavior_flow_utils.h"

using namespace bflow;

class NodeGraphTest : public ::testing::Test {
 protected:
  NodeGraph bf_graph;
  const NodeId StartNodeId = "start";
  const NodeId NextNodeId1 = "next1";
  const NodeId NextNodeId2 = "next2";
  const NodeId FailEndNodeId = "fail_end";
  const NodeId SuccessEndNodeId = "success_end";
  const NodeTypeId TestTaskNodeType = "task";
  const NodeTypeId TestConditionNodeType = "condition";
  const NodeTypeId SuccessNodeType = "success";
  const NodeTypeId FailureNodeType = "failure";
  const ResultId SuccessResultId = "Success";
  const ResultId FailureResultId = "Failure";
  const ResultId TrueResultId = "True";
  const ResultId FalseResultId = "False";
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
  const NodeGraph::NodeDescription FailEndNode = {FailEndNodeId, FailureNodeType, {}};
  const NodeGraph::NodeDescription SuccessEndNode = {SuccessEndNodeId, SuccessNodeType, {}};

  void addNodeTypesToGraph() {
    bf_graph.addNodeType({TestConditionNodeType, {TrueResultId, FalseResultId}});
    bf_graph.addNodeType({TestTaskNodeType, {SuccessResultId, FailureResultId}});
    bf_graph.addNodeType({FailureNodeType, {}});
    bf_graph.addNodeType({SuccessNodeType, {}});
  };
};

TEST_F(NodeGraphTest, StartNode) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  EXPECT_EQ(bf_graph.getStartNode().node_id, StartNode.node_id);
}

TEST_F(NodeGraphTest, SingleTransition) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  EXPECT_EQ(bf_graph.getNextNode(StartNode.node_id, TrueResultId).node_id, NextNode1.node_id);
}

TEST_F(NodeGraphTest, MultipleTransitions) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
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
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  EXPECT_ANY_THROW(bf_graph.getNextNode("nonexistent", SuccessResultId));
}

TEST_F(NodeGraphTest, NonExistentTransition) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  EXPECT_ANY_THROW(bf_graph.getNextNode(StartNode.node_id, "nonexistent"));
}

TEST_F(NodeGraphTest, DestinationNodeNotRegistered) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  EXPECT_ANY_THROW(bf_graph.getNextNode(StartNode.node_id, TrueResultId));
}

TEST_F(NodeGraphTest, AddDuplicateNodeId) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addNode(NextNode1);
  EXPECT_ANY_THROW(bf_graph.addNode(NextNode1));
}

TEST_F(NodeGraphTest, AddDuplicateNodeIdAsStartNode) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  EXPECT_ANY_THROW(bf_graph.addNode(StartNode));
}

TEST_F(NodeGraphTest, AddTwoStartNodes) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  EXPECT_ANY_THROW(bf_graph.addStartNode(NextNode1));
}

TEST_F(NodeGraphTest, ValidGraph) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(NextNode2);
  bf_graph.addNode(FailEndNode);
  bf_graph.addNode(SuccessEndNode);
  EXPECT_NO_THROW(bf_graph.validateThatGraphIsComplete());
}

TEST_F(NodeGraphTest, InvalidGraphMissingTransitionDestination) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(NextNode2);
  bf_graph.addNode(FailEndNode);
  // No SuccessEndNode
  EXPECT_ANY_THROW(bf_graph.validateThatGraphIsComplete());
}

TEST_F(NodeGraphTest, InvalidGraphMissingStartNode) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(NextNode2);
  bf_graph.addNode(FailEndNode);
  bf_graph.addNode(SuccessEndNode);
  EXPECT_ANY_THROW(bf_graph.validateThatGraphIsComplete());
}

TEST_F(NodeGraphTest, InvalidGraphWithUnreachableNode) {
  const NodeGraph::NodeDescription UnreachableNode = {"OrphanNode",
                                                      TestTaskNodeType,
                                                      {
                                                          {SuccessResultId, SuccessEndNodeId},
                                                          {FailureResultId, FailEndNodeId},
                                                      }};
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(NextNode2);
  bf_graph.addNode(FailEndNode);
  bf_graph.addNode(SuccessEndNode);
  bf_graph.addNode(UnreachableNode);
  EXPECT_ANY_THROW(bf_graph.validateThatGraphIsComplete());
}

TEST_F(NodeGraphTest, GraphWithCircularConnections) {
  const NodeId NextNodeId3 = "next3";
  const NodeGraph::NodeDescription CircularNextNode2 = {NextNodeId2,
                                                        TestTaskNodeType,
                                                        {
                                                            {SuccessResultId, NextNodeId3},
                                                            {FailureResultId, NextNodeId2},
                                                        }};
  const NodeGraph::NodeDescription CircularNextNode3 = {NextNodeId3,
                                                        TestTaskNodeType,
                                                        {
                                                            {SuccessResultId, SuccessEndNodeId},
                                                            {FailureResultId, NextNodeId1},
                                                        }};
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  bf_graph.addNode(NextNode1);
  bf_graph.addNode(CircularNextNode2);
  bf_graph.addNode(CircularNextNode3);
  bf_graph.addNode(FailEndNode);
  bf_graph.addNode(SuccessEndNode);
  EXPECT_NO_THROW(bf_graph.validateThatGraphIsComplete());
  EXPECT_EQ(bf_graph.getStartNode().node_id, StartNode.node_id);
  EXPECT_EQ(bf_graph.getNextNode(StartNode.node_id, TrueResultId).node_id, NextNode1.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode1.node_id, SuccessResultId).node_id,
            CircularNextNode2.node_id);
  EXPECT_EQ(bf_graph.getNextNode(CircularNextNode2.node_id, SuccessResultId).node_id,
            CircularNextNode3.node_id);
  EXPECT_EQ(bf_graph.getNextNode(CircularNextNode2.node_id, FailureResultId).node_id,
            CircularNextNode2.node_id);
  EXPECT_EQ(bf_graph.getNextNode(CircularNextNode3.node_id, SuccessResultId).node_id,
            SuccessEndNode.node_id);
  EXPECT_EQ(bf_graph.getNextNode(CircularNextNode3.node_id, FailureResultId).node_id,
            NextNode1.node_id);
}

TEST_F(NodeGraphTest, EmptyGraph) {
  bf_graph = NodeGraph();
  EXPECT_ANY_THROW(bf_graph.validateThatGraphIsComplete());
}

TEST_F(NodeGraphTest, InvalidGraphWithOnlyStartNode) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(StartNode);
  EXPECT_ANY_THROW(bf_graph.validateThatGraphIsComplete());
}

TEST_F(NodeGraphTest, ValidGraphWithSingleNodeNoTransitions) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  bf_graph.addStartNode(FailEndNode);
  EXPECT_NO_THROW(bf_graph.validateThatGraphIsComplete());
}

TEST_F(NodeGraphTest, AddNodeWithUnregisteredNodeType) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  EXPECT_ANY_THROW(bf_graph.addNode({NextNodeId1, "unregistered_type", {}}));
}

TEST_F(NodeGraphTest, AddNodeWithTransitionResultIdThatDoesntMatchNodeType) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  EXPECT_ANY_THROW(bf_graph.addNode({NextNodeId1,
                                     TestTaskNodeType,
                                     {
                                         {"InvalidResultId", NextNodeId2},
                                     }}));
}

TEST_F(NodeGraphTest, AddNodeWithIncompleteTransitionResultIds) {
  bf_graph = NodeGraph();
  addNodeTypesToGraph();
  EXPECT_ANY_THROW(bf_graph.addNode({NextNodeId1,
                                     TestTaskNodeType,
                                     {
                                         {SuccessResultId, NextNodeId2},
                                     }}));
}

TEST_F(NodeGraphTest, GraphConstructedFromContructorArguments) {
  bf_graph = NodeGraph(
      {
          {TestConditionNodeType, {TrueResultId, FalseResultId}},
          {TestTaskNodeType, {SuccessResultId, FailureResultId}},
          {FailureNodeType, {}},
          {SuccessNodeType, {}},
      },
      {
          StartNode,
          NextNode1,
          NextNode2,
          FailEndNode,
          SuccessEndNode,
      },
      StartNodeId);
  EXPECT_NO_THROW(bf_graph.validateThatGraphIsComplete());
  EXPECT_EQ(bf_graph.getStartNode().node_id, StartNode.node_id);
  EXPECT_EQ(bf_graph.getNextNode(StartNode.node_id, TrueResultId).node_id, NextNode1.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode1.node_id, SuccessResultId).node_id, NextNode2.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode2.node_id, FailureResultId).node_id, FailEndNode.node_id);
  EXPECT_EQ(bf_graph.getNextNode(NextNode2.node_id, SuccessResultId).node_id,
            SuccessEndNode.node_id);
}

TEST_F(NodeGraphTest, GraphConstructedFromContructorArgumentsMissingStartNode) {
  EXPECT_ANY_THROW(bf_graph = NodeGraph(
                       {
                           {TestConditionNodeType, {TrueResultId, FalseResultId}},
                           {TestTaskNodeType, {SuccessResultId, FailureResultId}},
                           {FailureNodeType, {}},
                           {SuccessNodeType, {}},
                       },
                       {
                           NextNode1,
                           NextNode2,
                           FailEndNode,
                           SuccessEndNode,
                       },
                       StartNodeId));
}