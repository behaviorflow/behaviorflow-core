// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__JSON_GRAPH_PARSER_H_
#define BEHAVIOR_FLOW__JSON_GRAPH_PARSER_H_

#include <filesystem>
#include <string>
#include "node_graph.h"

namespace bflow {
NodeGraph jsonFileToNodeGraph(const std::filesystem::path& json_file_path);
NodeGraph jsonStringToNodeGraph(const std::string& json_string);
}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__JSON_GRAPH_PARSER_H_