#include "behavior_flow_engine.h"
#include <cassert>

namespace bflow {

BehaviorFlowEngine::BehaviorFlowEngine(NodeRegistry&& registry)
    : node_registry_(std::move(registry)),
      node_instance_provider_(std::move(node_registry_)),
      graph_file_parser_(std::make_unique<GraphFileParserJson>()) {}

BehaviorFlowResult BehaviorFlowEngine::execute(const std::filesystem::path& graph_path) {
  NodeGraph graph = graph_file_parser_->parseGraphFile(graph_path);
  return execute(graph);
}

BehaviorFlowResult BehaviorFlowEngine::execute(const NodeGraph& graph) {
  ScopeGuard guard{[this] { node_instance_provider_.clear(); }};
  graph.validateThatGraphIsComplete();
  instantiateGraphNodes(graph);
  NodeGraph::NodeDescription current_node = graph.getStartNode();
  ResultId result_id;
  while (!isTerminalNodeType(current_node.node_type)) {
    const NodeWithMetadata& node_instance =
        node_instance_provider_.getNodeInstance(current_node.node_id, current_node.node_type);
    result_id = node_instance.node_instance->execute();
    current_node = graph.getNextNode(current_node.node_id, result_id);
  }
  const NodeGraph::NodeDescription final_node = current_node;
  if (final_node.node_type == SuccessNodeTypeId) {
    return BehaviorFlowResult::Success;
  } else {
    return BehaviorFlowResult::Failure;
  }
}

void BehaviorFlowEngine::setGraphFileParser(std::unique_ptr<GraphFileParserInterface> parser) {
  graph_file_parser_ = std::move(parser);
}

void BehaviorFlowEngine::instantiateGraphNodes(const NodeGraph& graph) {
  // Instantiate all of the nodes up front to catch any issues before execution begins
  // Todo: We could provide a configuration option to lazy load
  try {
    for (const auto& node : graph.getAllNodes()) {
      assert(node.first == node.second.node_id);
      const NodeWithMetadata& node_instance = node_instance_provider_.getNodeInstance(node.first, node.second.node_type);
      assert(node.first == node_instance.metadata.node_instance_id);
      assert(node.second.node_type == node_instance.metadata.node_type_id);
      // todo: once return types are implemented for BehaviorFlowNodeBase, validate that the return
      // 	types match the transitions in the graph
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("There was a problem instantiating the nodes from the node graph: " + std::string(e.what()));
  }
}

}  // namespace bflow