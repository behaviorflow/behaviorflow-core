// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "graph_file_parser_json.h"
#include "node_graph.h"
#include "testing_utils.h"
#include "utils/behavior_flow_types.h"

using namespace bflow;
using namespace bflow::test;

namespace {
const std::string TestValidGraphJson = R"json(
{
  "node_types": [
    {
      "node_type_id": "condition",
      "result_ids": ["True", "False"]
    },
    {
      "node_type_id": "task",
      "result_ids": ["Success", "Failure"]
    },
    {
      "node_type_id": "success",
      "result_ids": []
    },
    {
      "node_type_id": "failure",
      "result_ids": []
    }
  ],
  "nodes": [
    {
      "node_id": "start",
      "node_type": "condition",
      "transitions": {
        "True": "next1",
        "False": "fail_end"
      }
    },
    {
      "node_id": "next1",
      "node_type": "task",
      "transitions": {
        "Success": "next2",
        "Failure": "fail_end"
      }
    },
    {
      "node_id": "next2",
      "node_type": "task",
      "transitions": {
        "Success": "success_end",
        "Failure": "fail_end"
      }
    },
    {
      "node_id": "fail_end",
      "node_type": "failure",
      "transitions": {}
    },
    {
      "node_id": "success_end",
      "node_type": "success",
      "transitions": {}
    }
  ],
  "start_node_id": "start"
}
)json";

void ExpectValidGraphStructure(const NodeGraph& graph) {
  EXPECT_EQ(graph.getStartNode().node_id, NodeId("start"));
  EXPECT_EQ(graph.getNextNode(NodeId("start"), ResultId("True")).node_id, NodeId("next1"));
  EXPECT_EQ(graph.getNextNode(NodeId("start"), ResultId("False")).node_id, NodeId("fail_end"));
  EXPECT_EQ(graph.getNextNode(NodeId("next1"), ResultId("Success")).node_id, NodeId("next2"));
  EXPECT_EQ(graph.getNextNode(NodeId("next1"), ResultId("Failure")).node_id, NodeId("fail_end"));
  EXPECT_EQ(graph.getNextNode(NodeId("next2"), ResultId("Success")).node_id, NodeId("success_end"));
  EXPECT_EQ(graph.getNextNode(NodeId("next2"), ResultId("Failure")).node_id, NodeId("fail_end"));
  EXPECT_NO_THROW(graph.validateThatGraphIsComplete());
  const auto& nodes = graph.getAllNodes();
  EXPECT_EQ(nodes.size(), 5);
  EXPECT_TRUE(nodes.count(NodeId("start")));
  EXPECT_TRUE(nodes.count(NodeId("next1")));
  EXPECT_TRUE(nodes.count(NodeId("next2")));
  EXPECT_TRUE(nodes.count(NodeId("fail_end")));
  EXPECT_TRUE(nodes.count(NodeId("success_end")));
  const auto& node_types = graph.getAllNodeTypes();
  EXPECT_EQ(node_types.size(), 4);
  EXPECT_TRUE(node_types.count(NodeTypeId("condition")));
  EXPECT_TRUE(node_types.count(NodeTypeId("task")));
  EXPECT_TRUE(node_types.count(NodeTypeId("success")));
  EXPECT_TRUE(node_types.count(NodeTypeId("failure")));
}
}  // namespace

TEST(GraphFileParserJson, ValidGraph) {
  NodeGraph graph = GraphFileParserJson().parseGraphString(TestValidGraphJson);
  ExpectValidGraphStructure(graph);
}

TEST(GraphFileParserJson, EmptyString) {
  std::string graph_json = "";
  EXPECT_ANY_THROW(GraphFileParserJson().parseGraphString(graph_json));
}

TEST(GraphFileParserJson, JsonMissingClosingBrace) {
  std::string graph_json = R"json(
{
  "node_types": [
    {
      "node_type_id": "condition",
      "result_ids": ["True", "False"]
    }
  ],
  "nodes": [
    {
      "node_id": "start",
      "node_type": "condition",
      "transitions": {
        "True": "next1",
        "False": "fail_end"
      }
    },
  ],
  "start_node_id": "start"
)json";  // Missing closing brace
  EXPECT_ANY_THROW(GraphFileParserJson().parseGraphString(graph_json));
}

TEST(GraphFileParserJson, MissingFields) {
  std::string graph_json = R"json(
{
  "node_types": [
    {
      "node_type_id": "condition"
    }
  ],
  "nodes": [
    {
      "node_id": "start",
      "node_type": "condition"
    }
  ],
  "start_node_id": "start"
})json";  // Missing result_ids and transitions
  EXPECT_ANY_THROW(GraphFileParserJson().parseGraphString(graph_json));
}

TEST(GraphFileParserJson, NoStartNode) {
  std::string graph_json = R"json(
{
  "node_types": [
    {
      "node_type_id": "condition",
      "result_ids": ["True", "False"]
    }
  ],
  "nodes": [
    {
      "node_id": "start",
      "node_type": "  condition",
      "transitions": {
        "True": "next1",
        "False": "fail_end"
      }
    }
  ]
})json";  // Missing start_node_id
  EXPECT_ANY_THROW(GraphFileParserJson().parseGraphString(graph_json));
}

TEST(GraphFileParserJson, EmptyFile) {
  ScopedTempFile temp_file("", ".json");
  EXPECT_ANY_THROW(GraphFileParserJson().parseGraphFile(temp_file.path()));
}

TEST(GraphFileParserJson, ValidFile) {
  ScopedTempFile temp_file(TestValidGraphJson, ".json");
  NodeGraph graph = GraphFileParserJson().parseGraphFile(temp_file.path());
  ExpectValidGraphStructure(graph);
}