// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_

#include <string>
#include <memory>
#include <vector>

namespace bflow
{
class BehaviorFlowNodeBase;

using NodeId = std::string;
using NodeTypeId = std::string;
using ResultId = std::string;

struct NodeTypeMetadata
{
	NodeTypeId node_type_id;
	std::vector<ResultId> valid_result_ids;
	auto operator<=>(const NodeTypeMetadata&) const = default;
};

} // namespace bflow


#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_