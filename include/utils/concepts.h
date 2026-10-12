// Copyright (c) 2025, Mitch Adams

#ifndef BEHAVIOR_FLOW__CONCEPTS_H_
#define BEHAVIOR_FLOW__CONCEPTS_H_

#include <concepts>
#include <functional>
#include <type_traits>

#include "utils/return_types.h"

namespace bflow {

template <typename T>
struct unwrap_reference_wrapper {
  using type = T;
};

template <typename T>
struct unwrap_reference_wrapper<std::reference_wrapper<T>> {
  using type = T&;
};

template <typename T>
using unwrap_reference_wrapper_t = typename unwrap_reference_wrapper<std::remove_cvref_t<T>>::type;

template <typename Func, typename ReturnT>
concept NodeFunction =
    std::invocable<unwrap_reference_wrapper_t<Func>> &&
    std::is_same_v<std::invoke_result_t<unwrap_reference_wrapper_t<Func>>, ReturnT>;

template <typename Func>
concept VoidNodeFunction = NodeFunction<Func, void>;

template <typename Func>
concept AsyncSimpleNodeFunction = NodeFunction<Func, AsyncSimpleNodeResult>;

template <typename Func>
concept BoolNodeFunction = NodeFunction<Func, bool>;

template <typename ResultTInterpreter, typename ResultT>
concept ResultTypeInterpreter = requires {
  { ResultTInterpreter::validResultIds() } -> std::same_as<std::vector<ResultId>>;
  { ResultTInterpreter::toNodeReturnType(std::declval<ResultT>()) } -> std::same_as<ReturnType>;
};

template <typename T>
concept BehaviorFlowNodeType = std::is_base_of<BehaviorFlowNodeBase, T>::value;

template <typename ObjectT, auto MethodPtr, typename ResultT>
concept ObjectFcn = std::is_invocable_v<decltype(MethodPtr), ObjectT&> &&
                    std::is_same_v<std::invoke_result_t<decltype(MethodPtr), ObjectT&>, ResultT>;

template <typename ObjectT, auto MethodPtr>
concept OptionalVoidObjectFcn =
    std::is_null_pointer_v<decltype(MethodPtr)> ||
    (std::is_invocable_v<decltype(MethodPtr), ObjectT&> &&
     std::is_same_v<std::invoke_result_t<decltype(MethodPtr), ObjectT&>, void>);

template <typename ObjectT, auto ExecutionFcn, typename ResultTInterpreter,
          auto InitFcn = nullptr, auto TeardownFcn = nullptr>
concept ValidObjectWrapperNode =
    ResultTypeInterpreter<ResultTInterpreter, typename ResultTInterpreter::ResultType> &&
    ObjectFcn<ObjectT, ExecutionFcn, typename ResultTInterpreter::ResultType> &&
    OptionalVoidObjectFcn<ObjectT, InitFcn> &&
    OptionalVoidObjectFcn<ObjectT, TeardownFcn>;

}  // namespace bflow

#endif  // BEHAVIOR_FLOW__CONCEPTS_H_