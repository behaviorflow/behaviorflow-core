#include "graph_file_parser_json.h"

#include "json.hpp"

namespace bflow {

static void validateRequiredFields(const nlohmann::json& json,
                            const std::vector<std::string>& required_fields,
                            const std::string& context_description) {
  for (const auto& field : required_fields) {
    if (!json.contains(field)) {
      throw std::runtime_error("Missing required field: '" + field + "' in " + context_description);
    }
  }
}

NodeGraph GraphFileParserJson::parseGraphString(const std::string& graph) {
  try {
    nlohmann::json json = nlohmann::json::parse(graph);
    validateRequiredFields(json, {"node_types", "nodes", "start_node_id"}, "root object");
    // Parse node_types
    std::vector<NodeGraph::NodeTypeDescription> node_types;
    for (const auto& node_type : json["node_types"]) {
      validateRequiredFields(node_type, {"node_type_id", "result_ids"}, "node_types element");
      NodeTypeId node_type_id(node_type["node_type_id"].get<std::string>());
      std::vector<ResultId> result_ids;
      for (const auto& result_id : node_type["result_ids"]) {
        result_ids.emplace_back(result_id.get<std::string>());
      }
      node_types.emplace_back(
          NodeGraph::NodeTypeDescription{.node_type_id = node_type_id, .result_ids = result_ids});
    }

    // Parse nodes
    std::vector<NodeGraph::NodeDescription> nodes;
    for (const auto& node : json["nodes"]) {
      validateRequiredFields(node, {"node_id", "node_type", "transitions"}, "nodes element");
      NodeId node_id(node["node_id"].get<std::string>());
      NodeTypeId node_type(node["node_type"].get<std::string>());
      std::map<ResultId, NodeId> transitions;
      for (auto it = node["transitions"].begin(); it != node["transitions"].end(); ++it) {
        transitions.emplace(ResultId(it.key()), NodeId(it.value().get<std::string>()));
      }
      nodes.emplace_back(NodeGraph::NodeDescription{
          .node_id = node_id, .node_type = node_type, .transitions = transitions});
    }

    NodeId start_node_id(json["start_node_id"].get<std::string>());

    return NodeGraph(node_types, nodes, start_node_id);
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("Failure to parse Node Graph JSON: ") + e.what());
  } catch (...) {
    throw std::runtime_error(std::string("Unknown error while processing Node Graph JSON."));
  }
}

}  // namespace bflow
