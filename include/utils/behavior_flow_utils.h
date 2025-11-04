// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_UTILS_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_UTILS_H_

#include <functional>

#include "utils/behavior_flow_types.h"

namespace bflow {
inline const NodeTypeId SuccessNodeTypeId = "Success";
inline const NodeTypeId FailureNodeTypeId = "Failure";

// ScopeGuard utility for executing a lambda on scope exit
class ScopeGuard {
 public:
  explicit ScopeGuard(std::function<void()> on_exit) : on_exit_(std::move(on_exit)) {}
  ScopeGuard(const ScopeGuard&) = delete;
  ScopeGuard& operator=(const ScopeGuard&) = delete;
  ~ScopeGuard() { on_exit_(); }

 private:
  std::function<void()> on_exit_;
};

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_UTILS_H_