// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_ENGINE_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_ENGINE_H_

#include <filesystem>

#include "graph_file_parser_interface.h"
#include "graph_file_parser_json.h"
#include "node_graph.h"
#include "node_instance_provider.h"
#include "node_registry.h"

namespace bflow {

enum class BehaviorFlowResult {
  Success,
  Failure,
};

class BehaviorFlowEngine {
 public:
  BehaviorFlowEngine() = default;
  explicit BehaviorFlowEngine(NodeRegistry&& registry);

  BehaviorFlowResult execute(const std::filesystem::path& graph_path);
  BehaviorFlowResult execute(const NodeGraph& graph);
  void setGraphFileParser(std::unique_ptr<GraphFileParserInterface> parser);

 private:
  void instantiateGraphNodes(const NodeGraph& graph);
  NodeRegistry node_registry_;
  NodeInstanceProvider node_instance_provider_;
  std::unique_ptr<GraphFileParserInterface> graph_file_parser_;
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_ENGINE_H_