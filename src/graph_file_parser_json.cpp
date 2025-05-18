#include "graph_file_parser_json.h"

namespace bflow {

std::unique_ptr<NodeGraph> GraphFileParserJson::parseGraphString(std::string graph) {
  return std::make_unique<NodeGraph>();
}

}  // namespace bflow
