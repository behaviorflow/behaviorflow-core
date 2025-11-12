// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__STANDARD_NODE_LIBRARY_H_
#define BEHAVIOR_FLOW__STANDARD_NODE_LIBRARY_H_

#include "node_registry.h"

namespace bflow {

inline const NodeTypeId SuccessNodeTypeId = "Success";
inline const NodeTypeId FailureNodeTypeId = "Failure";
// ExecutionCountExceedsThresholdNodeTypeId

void register_standard_library(NodeRegistry& registry);

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_GRAPH_H_