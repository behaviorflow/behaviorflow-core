#include "graph_file_parser_interface.h"

namespace behaviorflow {

std::unique_ptr<NodeGraph> GraphFileParserInterface::parseGraphFile(
    std::string graph_file_path) {
  std::string graph_string = getStringFromFile(graph_file_path);
  return parseGraphString(graph_string);
}

std::string GraphFileParserInterface::getStringFromFile(std::string graph_file_path) {
  std::ifstream file(graph_file_path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + graph_file_path);
  }
  std::string graph_string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
  return graph_string;
}

}  // namespace behaviorflow