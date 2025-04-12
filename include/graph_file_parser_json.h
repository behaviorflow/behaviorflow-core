// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__GRAPH_FILE_PARSER_H_
#define BEHAVIOR_FLOW__GRAPH_FILE_PARSER_H_

#include <memory>

#include "graph_file_parser_interface.h"

namespace behaviorflow {
class GraphFileParserJson : public GraphFileParserInterface {
public:
  GraphFileParserJson() = default;
  std::unique_ptr<NodeGraph> parseGraphString(std::string graph) override;
};
}  // end namespace behaviorflow

#endif  // BEHAVIOR_FLOW__GRAPH_FILE_PARSER_H_