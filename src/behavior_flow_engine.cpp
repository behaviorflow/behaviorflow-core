#include "behavior_flow_engine.h"

#include <algorithm>
#include <cassert>
#include <optional>
#include <string>

#include "standard_node_library.h"
#include "utils/behavior_flow_types.h"
#include "utils/behavior_flow_utils.h"

namespace bflow {

BehaviorFlowEngine::BehaviorFlowEngine(NodeRegistry&& registry)
    : node_instance_provider_(std::move(registry)),
      graph_file_parser_(std::make_unique<GraphFileParserJson>()) {}

BehaviorFlowResult BehaviorFlowEngine::execute(const std::filesystem::path& graph_path) {
  NodeGraph graph = graph_file_parser_->parseGraphFile(graph_path);
  return execute(graph);
}

BehaviorFlowResult BehaviorFlowEngine::execute(const NodeGraph& graph) {
  ScopeGuard guard{[this] { node_instance_provider_.clear(); }};
  graph.validateThatGraphIsComplete();
  validateThatGraphMatchesRegisteredNodeTypes(graph);
  instantiateGraphNodes(graph);
  NodeGraph::NodeDescription current_node = graph.getStartNode();
  ResultId result_id;
  while (!isTerminalNodeType(current_node)) {
    BehaviorFlowNodeBase& node_instance =
        node_instance_provider_.getNodeInstance(current_node.node_id, current_node.node_type);
    result_id = node_instance.execute();
    current_node = graph.getNextNode(current_node.node_id, result_id);
  }
  const NodeGraph::NodeDescription& final_node = current_node;
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
    for (const auto& [node_id, node_description] : graph.getAllNodes()) {
      assert(node_id == node_description.node_id);  // Sanity check
      node_instance_provider_.getNodeInstance(node_id, node_description.node_type);
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("There was a problem instantiating the nodes from the node graph: " +
                             std::string(e.what()));
  }
}

bool BehaviorFlowEngine::isTerminalNodeType(
    const NodeGraph::NodeDescription& node_description) const {
  return node_description.transitions.empty();
}

void BehaviorFlowEngine::validateThatGraphMatchesRegisteredNodeTypes(const NodeGraph& graph) const {
  for (const auto& [node_type_id, node_type_description] : graph.getAllNodeTypes()) {
    std::optional<NodeTypeMetadata> registered_node_type_metadata =
        node_instance_provider_.getNodeTypeMetadata(node_type_id);
    if (!registered_node_type_metadata.has_value()) {
      throw std::runtime_error("Node type '" + node_type_id +
                               "' is defined in the node graph but has not been registered.");
    }
    // Check that result IDs match
    if (node_type_description.result_ids.size() !=
            registered_node_type_metadata->valid_result_ids.size() ||
        !std::is_permutation(node_type_description.result_ids.begin(),
                             node_type_description.result_ids.end(),
                             registered_node_type_metadata->valid_result_ids.begin())) {
      std::string graph_result_ids_str = joinStrings(node_type_description.result_ids, ", ");
      std::string registered_result_ids_str =
          joinStrings(registered_node_type_metadata->valid_result_ids, ", ");
      joinStrings(registered_node_type_metadata->valid_result_ids, ", ");
      throw std::runtime_error("There is a mismatch between the result ids of the node type '" +
                               node_type_id + "' as defined in the graph (" + graph_result_ids_str +
                               ") and the result ids of the registered node type (" +
                               registered_result_ids_str + ").");
    }
  }
}
}  // namespace bflow