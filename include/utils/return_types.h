// Copyright (c) 2026, Mitch Adams

#ifndef BEHAVIOR_FLOW__RETURN_TYPES_H_
#define BEHAVIOR_FLOW__RETURN_TYPES_H_

#include "utils/behavior_flow_types.h"

namespace bflow {

inline const ResultId SimpleNodeResultId{""};
inline const ResultId DecisionNodeTrueResultId{"Yes"};
inline const ResultId DecisionNodeFalseResultId{"No"};

enum class SimpleNodeResult { Running, Finished };

class SimpleResultTypeInterpreter {
 public:
  static ReturnType toNodeReturnType(SimpleNodeResult result) {
    return result == SimpleNodeResult::Running ? ReturnType::StillRunning()
                                               : ReturnType(SimpleNodeResultId);
  }
  static std::vector<ResultId> validResultIds() { return {SimpleNodeResultId}; }
};

class BoolResultTypeInterpreter {
 public:
  static ReturnType toNodeReturnType(bool result) {
    return result ? ReturnType(DecisionNodeTrueResultId) : ReturnType(DecisionNodeFalseResultId);
  }

  static std::vector<ResultId> validResultIds() {
    return {DecisionNodeTrueResultId, DecisionNodeFalseResultId};
  }
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__RETURN_TYPES_H_