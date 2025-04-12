// Copyright (c) 2024, Mitch Adams

#ifndef BEHAVIOR_FLOW__GRAPH_FILE_PARSER_INTERFACE_H_
#define BEHAVIOR_FLOW__GRAPH_FILE_PARSER_INTERFACE_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "node_graph.h"

namespace behaviorflow {
/**
 * @brief Interface for parsing a file representing a NodeGraph and returning a
 * NodeGraph object.
 */
class GraphFileParserInterface {
public:
  GraphFileParserInterface() = default;
  virtual ~GraphFileParserInterface() = default;
  /**
   * @brief Parse a graph file and return a NodeGraph object.
   * The default implementation reads the file into a string using getStringFromFile and then calls
   * parseGraphString. This can be overridden if needed.
   */
  virtual std::unique_ptr<NodeGraph> parseGraphFile(std::string graph_file_path);

  /**
   * @brief Parse a string representing a NodeGraph and return a NodeGraph object.
   */
  virtual std::unique_ptr<NodeGraph> parseGraphString(std::string graph) = 0;

 private:
  std::string getStringFromFile(std::string graph_file_path);
};
}  // end namespace behaviorflow

#endif  // BEHAVIOR_FLOW__GRAPH_FILE_PARSER_INTERFACE_H_