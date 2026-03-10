// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_
#define BEHAVIOR_FLOW__BEHAVIOR_FLOW_TYPES_H_

#include <memory>
#include <string>
#include <vector>

namespace bflow {

class BehaviorFlowNodeBase;

template <typename Tag>
struct Id {
 public:
  explicit Id(const std::string& v) : value_(v) {}
  Id() = default;
  bool operator==(const Id& other) const { return value_ == other.value(); }
  bool operator!=(const Id& other) const { return value_ != other.value(); }
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

struct NodeIdTag {};
struct NodeTypeIdTag {};
struct ResultIdTag {};
using NodeId = Id<NodeIdTag>;
using NodeTypeId = Id<NodeTypeIdTag>;
using ResultId = Id<ResultIdTag>;

class ReturnType {
  public:
    static ReturnType StillRunning() { return ReturnType(StillRunningTag{}); }
    explicit ReturnType(ResultId result_id) : result_id_(result_id) {}
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