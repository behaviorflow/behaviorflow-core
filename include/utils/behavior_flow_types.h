// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_

#include <memory>
#include <string>
#include <vector>

namespace bflow {

class BehaviorFlowNodeBase;

/**
 * @brief Wraps a string value as a strongly-typed identifier. Prevents confusion or misuse between
 * different types of identifiers.
 * @tparam Tag The type used to differentiate this identifier from others.
 */
template <typename Tag>
struct Id {
 public:
  explicit Id(const std::string& v) : value_(v) {}
  Id() = default;
  auto operator<=>(const Id&) const = default;
  bool empty() const { return value_.empty(); }
  const std::string& value() const { return value_; }

 private:
  std::string value_;
};

template <typename Tag>
inline std::ostream& operator<<(std::ostream& os, const Id<Tag>& id) {
  return os << id.value();
}

template <typename Tag>
inline std::string operator+(const std::string& lhs, const Id<Tag>& rhs) {
  return lhs + rhs.value();
}
template <typename Tag>
inline std::string operator+(const Id<Tag>& lhs, const std::string& rhs) {
  return lhs.value() + rhs;
}

/** Id for a node instance */
struct NodeIdTag {};
using NodeId = Id<NodeIdTag>;

/** Id for a node type */
struct NodeTypeIdTag {};
using NodeTypeId = Id<NodeTypeIdTag>;

/** Id for the result of a completed node execution */
struct ResultIdTag {};
using ResultId = Id<ResultIdTag>;

/**
 * @brief This is what is ultimately returned after a single execution tick of a node. It can either
 * represent a ResultId (which informs the subsequent node to execute), or it can indicate that
 * the node is still running and should be ticked again in the next cycle.
 */
class ReturnType {
 public:
  /** Creates a ReturnType indicating the node is still running. */
  static ReturnType StillRunning() { return ReturnType(StillRunningTag{}); }

  /**
   * Constructs a ReturnType representing a completed node execution with the given result_id.
   */
  explicit ReturnType(ResultId result_id) : result_id_(result_id) {}

  /** Constructs a ReturnType representing a completed node execution with the given result_id as a
   * string.
   */
  explicit ReturnType(std::string result_id_str) : result_id_(ResultId(result_id_str)) {}

  auto operator<=>(const ReturnType&) const = default;
  bool still_running() const { return still_running_; }
  const ResultId& result_id() const { return result_id_; }

 private:
  struct StillRunningTag {};
  explicit ReturnType(StillRunningTag) : still_running_(true) {}
  ResultId result_id_;
  bool still_running_{false};
};

struct NodeTypeMetadata {
  NodeTypeId node_type_id;
  std::vector<ResultId> valid_result_ids;
  auto operator<=>(const NodeTypeMetadata&) const = default;
};

}  // namespace bflow

namespace std {
template <typename Tag>
struct hash<::bflow::Id<Tag>> {
  std::size_t operator()(const ::bflow::Id<Tag>& id) const noexcept {
    return std::hash<std::string>{}(id.value());
  }
};
}  // namespace std

#endif  // BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_