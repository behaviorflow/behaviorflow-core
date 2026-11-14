// Copyright (c) 2026, Mitch Adams

#ifndef BEHAVIOR_FLOW__RETURN_TYPES_H_
#define BEHAVIOR_FLOW__RETURN_TYPES_H_

#include "utils/behavior_flow_types.h"

namespace bflow {

// Simple node return type
inline const ResultId SimpleNodeResultId{""};

enum class AsyncSimpleNodeResult { Running, Finished };

struct AsyncSimpleNodeReturnTypeInterpreter {
  static NodeTickResult toNodeTickResult(AsyncSimpleNodeResult result) {
    return result == AsyncSimpleNodeResult::Running ? NodeTickResult::StillRunning()
                                                    : NodeTickResult(SimpleNodeResultId);
  }
  static std::vector<ResultId> validResultIds() { return {SimpleNodeResultId}; }
};

// boolean return type
inline const ResultId DecisionNodeTrueResultId{"Yes"};
inline const ResultId DecisionNodeFalseResultId{"No"};

struct BoolReturnTypeInterpreter {
  static NodeTickResult toNodeTickResult(bool result) {
    return result ? NodeTickResult(DecisionNodeTrueResultId)
                  : NodeTickResult(DecisionNodeFalseResultId);
  }

  static std::vector<ResultId> validResultIds() {
    return {DecisionNodeTrueResultId, DecisionNodeFalseResultId};
  }
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__RETURN_TYPES_H_