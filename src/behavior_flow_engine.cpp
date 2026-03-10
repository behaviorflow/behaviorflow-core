#include "behavior_flow_engine.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <optional>
#include <string>
#include <thread>

#include "json_graph_parser.h"
#include "standard_node_library.h"
#include "utils/behavior_flow_types.h"
#include "utils/behavior_flow_utils.h"

namespace bflow {

BehaviorFlowEngine::BehaviorFlowEngine(NodeRegistry&& registry, NodeGraph&& graph)
    : node_instance_provider_(std::move(registry)), graph_(std::move(graph)) {
  graph_.validateThatGraphIsComplete();
  validateThatGraphMatchesRegisteredNodeTypes();
  instantiateGraphNodes();
  current_node_ = graph_.getStartNode();
}

BehaviorFlowEngine::BehaviorFlowEngine(NodeRegistry&& registry,
                                       const std::filesystem::path& graph_json_path)
    : BehaviorFlowEngine(std::move(registry), std::move(jsonFileToNodeGraph(graph_json_path))) {}

BehaviorFlowResult BehaviorFlowEngine::execute(std::chrono::milliseconds tick_rate) {
  while (true) {
    std::chrono::steady_clock::time_point tick_start_time = std::chrono::steady_clock::now();
    const BehaviorFlowResult tick_result = tick();
    if (tick_result != BehaviorFlowResult::Running) {
      return tick_result;
    }
    // todo: log warning if tick execution time exceeds tick rate
    std::this_thread::sleep_for(tick_rate - (std::chrono::steady_clock::now() - tick_start_time));
  }
}

BehaviorFlowResult BehaviorFlowEngine::tick() {
  if (isTerminalNodeType(current_node_)) {
    if (current_node_.node_type == SuccessNodeTypeId) {
      return BehaviorFlowResult::Success;
    } else {
      return BehaviorFlowResult::Failure;
    }
    reset();
  }
  BehaviorFlowNodeBase& node_instance =
      node_instance_provider_.getNodeInstance(current_node_.node_id, current_node_.node_type);
  ReturnType node_return = node_instance.execute();
  if (!node_return.still_running()) {
    current_node_ = graph_.getNextNode(current_node_.node_id, node_return.result_id());
  }
  return BehaviorFlowResult::Running;
}

void BehaviorFlowEngine::reset() {
  current_node_ = graph_.getStartNode();
  node_instance_provider_.clear();
}

bool BehaviorFlowEngine::isTerminalNodeType(
    const NodeGraph::NodeDescription& node_description) const {
  return node_description.transitions.empty();
}

void BehaviorFlowEngine::instantiateGraphNodes() {
  // Instantiate all of the nodes up front to catch any issues before execution begins
  // Todo: We could provide a configuration option to lazy load
  try {
    for (const auto& [node_id, node_description] : graph_.getAllNodes()) {
      assert(node_id == node_description.node_id);  // Sanity check
      node_instance_provider_.getNodeInstance(node_id, node_description.node_type);
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("There was a problem instantiating the nodes from the node graph: " +
                             std::string(e.what()));
  }
}

void BehaviorFlowEngine::validateThatGraphMatchesRegisteredNodeTypes() const {
  for (const auto& [node_type_id, node_type_description] : graph_.getAllNodeTypes()) {
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