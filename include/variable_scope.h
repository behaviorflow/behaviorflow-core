// Copyright (c) 2024, Mitch Adams

// includes variables and global variables for a given scope, mapped by key

#ifndef BEHAVIOR_FLOW__VARIABLE_SCOPE_H_
#define BEHAVIOR_FLOW__VARIABLE_SCOPE_H_

#include <any>
#include <map>
#include <string>
#include <vector>

namespace bflow {

class VariableScope {
public:
  VariableScope() = default;
  VariableScope(std::map<std::string, std::any> local_vars,
                std::shared_ptr<std::map<std::string, std::any>> global_vars);

  VariableScope getSubScope(std::vector<std::string> local_var_names);

  template <typename T>
  T getLocalVariable(std::string var_name) {}

  template <typename T>
  T getGlobalVariable(std::string var_name) {}

 private:
  std::map<std::string, std::any> local_vars_;
  std::shared_ptr<std::map<std::string, std::any>> global_vars_;
};

}  // end namespace bflow

#endif  // BEHAVIOR_FLOW__VARIABLE_SCOPE_H_