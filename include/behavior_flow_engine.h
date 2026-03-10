// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_ENGINE_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_ENGINE_H_

#include <filesystem>

#include "node_graph.h"
#include "node_instance_provider.h"
#include "node_registry.h"

namespace bflow {

enum class BehaviorFlowResult {
  Success,
  Failure,
  Running,
};

class BehaviorFlowEngine {
 public:
  BehaviorFlowEngine() = delete;
  BehaviorFlowEngine(NodeRegistry&& registry, NodeGraph&& graph);
  BehaviorFlowEngine(NodeRegistry&& registry,
                                         const std::filesystem::path& graph_json_path);

  BehaviorFlowResult execute(std::chrono::milliseconds tick_rate);
  BehaviorFlowResult tick();
  void reset();

 private:
  bool isTerminalNodeType(const NodeGraph::NodeDescription& node_description) const;
  void validateThatGraphMatchesRegisteredNodeTypes() const;
  void instantiateGraphNodes();
  NodeInstanceProvider node_instance_provider_;
  NodeGraph graph_;
  NodeGraph::NodeDescription current_node_;
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_ENGINE_H_