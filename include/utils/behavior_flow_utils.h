// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_UTILS_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_UTILS_H_

#include <concepts>
#include <functional>
#include <ranges>
#include <sstream>
#include <string>
#include <type_traits>

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

// Concept to ensure the range contains string-convertible elements
template <typename Range>
concept StringRange = std::ranges::input_range<Range> &&
                      requires(std::ranges::range_value_t<Range> value, std::ostream& os) {
                        { os << value } -> std::convertible_to<std::ostream&>;
                      };

                      // todo: not sure I want it inline
template <StringRange Range>
inline std::string joinStrings(Range&& strings, const std::string& delimiter = ", ",
                               bool quote_elements = true) {
  std::ostringstream result;
  bool first = true;
  for (const auto& str : strings) {
    if (!first) {
      result << delimiter;
    }
    if (quote_elements) {
      result << "\"" << str << "\"";
    } else {
      result << str;
    }
    first = false;
  }
  return result.str();
}

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_UTILS_H_