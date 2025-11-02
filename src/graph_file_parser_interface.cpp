#include "graph_file_parser_interface.h"

namespace bflow {

NodeGraph GraphFileParserInterface::parseGraphFile(
    const std::filesystem::path& graph_file_path) {
  std::string graph_string = getStringFromFile(graph_file_path);
  return parseGraphString(graph_string);
}

std::string GraphFileParserInterface::getStringFromFile(const std::filesystem::path& graph_file_path) {
  std::ifstream file(graph_file_path, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + graph_file_path.string());
  }
  std::string graph_string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
  return graph_string;
}

}  // namespace bflow