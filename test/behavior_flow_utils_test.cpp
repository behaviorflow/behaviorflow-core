// Copyright (c) 2025, Mitch Adams

#include "utils/behavior_flow_utils.h"

#include <gtest/gtest.h>

#include <array>
#include <list>
#include <string>
#include <vector>

using namespace bflow;

// ============================================================================
// ScopeGuard Tests
// ============================================================================

TEST(ScopeGuardTest, ExecutesOnScopeExit) {
  bool executed = false;
  {
    ScopeGuard guard([&executed]() { executed = true; });
    EXPECT_FALSE(executed);
  }
  EXPECT_TRUE(executed);
}

TEST(ScopeGuardTest, ExecutesOnException) {
  bool executed = false;
  try {
    ScopeGuard guard([&executed]() { executed = true; });
    throw std::runtime_error("test exception");
  } catch (const std::runtime_error&) {
    // Exception caught
  }
  EXPECT_TRUE(executed);
}

TEST(ScopeGuardTest, NestedScopeGuards) {
  std::vector<int> execution_order;
  {
    ScopeGuard guard1([&execution_order]() { execution_order.push_back(1); });
    {
      ScopeGuard guard2([&execution_order]() { execution_order.push_back(2); });
      EXPECT_TRUE(execution_order.empty());
    }
    EXPECT_EQ(execution_order.size(), 1);
    EXPECT_EQ(execution_order[0], 2);
  }
  EXPECT_EQ(execution_order.size(), 2);
  EXPECT_EQ(execution_order[0], 2);
  EXPECT_EQ(execution_order[1], 1);
}

// ============================================================================
// joinStrings Tests
// ============================================================================

TEST(JoinStringsTest, EmptyVector) {
  std::vector<std::string> empty;
  EXPECT_EQ(joinStrings(empty, ", ", false), "");
}

TEST(JoinStringsTest, SingleElement) {
  std::vector<std::string> single = {"hello"};
  EXPECT_EQ(joinStrings(single, ", ", false), "hello");
}

TEST(JoinStringsTest, MultipleElements) {
  std::vector<std::string> words = {"apple", "banana", "cherry"};
  EXPECT_EQ(joinStrings(words, ", ", false), "apple, banana, cherry");
}

TEST(JoinStringsTest, DifferentDelimiters) {
  std::vector<std::string> words = {"one", "two", "three"};
  EXPECT_EQ(joinStrings(words, "-", false), "one-two-three");
  EXPECT_EQ(joinStrings(words, " | ", false), "one | two | three");
  EXPECT_EQ(joinStrings(words, "", false), "onetwothree");
  EXPECT_EQ(joinStrings(words, " <--> ", false), "one <--> two <--> three");
  EXPECT_EQ(joinStrings(words, "\n", false), "one\ntwo\nthree");
}

TEST(JoinStringsTest, EmptyStrings) {
  std::vector<std::string> words = {"", "", ""};
  EXPECT_EQ(joinStrings(words, ", ", false), ", , ");
}

TEST(JoinStringsTest, MixedEmptyAndNonEmpty) {
  std::vector<std::string> words = {"hello", "", "world"};
  EXPECT_EQ(joinStrings(words, " ", false), "hello  world");
}

TEST(JoinStringsTest, WithSpecialCharacters) {
  std::vector<std::string> words = {"hello\nworld", "foo\tbar", "baz"};
  EXPECT_EQ(joinStrings(words, " - ", false), "hello\nworld - foo\tbar - baz");
}

TEST(JoinStringsTest, WorksWithArray) {
  std::array<std::string, 4> arr = {"first", "second", "third", "fourth"};
  EXPECT_EQ(joinStrings(arr, " ", false), "first second third fourth");
}

TEST(JoinStringsTest, WorksWithList) {
  std::list<std::string> lst = {"alpha", "beta", "gamma"};
  EXPECT_EQ(joinStrings(lst, "::", false), "alpha::beta::gamma");
}

TEST(JoinStringsTest, WorksWithIntegers) {
  std::vector<int> numbers = {1, 2, 3, 4, 5};
  EXPECT_EQ(joinStrings(numbers, ", ", false), "1, 2, 3, 4, 5");
}

TEST(JoinStringsTest, WorksWithDoubles) {
  std::vector<double> numbers = {1.5, 2.7, 3.9};
  EXPECT_EQ(joinStrings(numbers, " | ", false), "1.5 | 2.7 | 3.9");
}

// Custom type for testing operator<< with joinStrings
namespace {
struct CustomType {
  int value;
};

std::ostream& operator<<(std::ostream& os, const CustomType& ct) {
  return os << "Custom(" << ct.value << ")";
}
}  // namespace

TEST(JoinStringsTest, WorksWithMixedTypes) {
  std::vector<CustomType> items = {{1}, {2}, {3}};
  EXPECT_EQ(joinStrings(items, " - ", false), "Custom(1) - Custom(2) - Custom(3)");
}

TEST(JoinStringsTest, WorksWithRanges) {
  std::vector<std::string> words = {"alpha", "beta", "gamma", "delta"};
  // Test with a subrange (if C++20 ranges are available)
  auto subrange = words | std::views::take(2);
  EXPECT_EQ(joinStrings(subrange, " ", false), "alpha beta");
}

TEST(JoinStringsTest, WorksWithConstStrings) {
  const std::vector<std::string> words = {"const", "test"};
  EXPECT_EQ(joinStrings(words, " ", false), "const test");
}

TEST(JoinStringsTest, WorksWithStringView) {
  std::vector<std::string_view> views = {"view1", "view2", "view3"};
  EXPECT_EQ(joinStrings(views, "/", false), "view1/view2/view3");
}

// ============================================================================
// joinStrings Tests with Quotes
// ============================================================================

TEST(JoinStringsTest, WithQuotesEmptyVector) {
  std::vector<std::string> empty;
  EXPECT_EQ(joinStrings(empty, ", ", true), "");
}

TEST(JoinStringsTest, WithQuotesSingleElement) {
  std::vector<std::string> single = {"hello"};
  EXPECT_EQ(joinStrings(single, ", ", true), "\"hello\"");
}

TEST(JoinStringsTest, WithQuotesMultipleElements) {
  std::vector<std::string> words = {"apple", "banana", "cherry"};
  EXPECT_EQ(joinStrings(words, ", ", true), "\"apple\", \"banana\", \"cherry\"");
}

TEST(JoinStringsTest, WithQuotesDifferentDelimiters) {
  std::vector<std::string> words = {"one", "two", "three"};
  EXPECT_EQ(joinStrings(words, " | ", true), "\"one\" | \"two\" | \"three\"");
  EXPECT_EQ(joinStrings(words, "\n", true), "\"one\"\n\"two\"\n\"three\"");
}

TEST(JoinStringsTest, WithQuotesEmptyStrings) {
  std::vector<std::string> words = {"", "text", ""};
  EXPECT_EQ(joinStrings(words, ", ", true), "\"\", \"text\", \"\"");
}

TEST(JoinStringsTest, WithQuotesSpecialCharacters) {
  std::vector<std::string> words = {"hello\nworld", "foo\tbar"};
  EXPECT_EQ(joinStrings(words, " - ", true), "\"hello\nworld\" - \"foo\tbar\"");
}

TEST(JoinStringsTest, WithQuotesIntegers) {
  std::vector<int> numbers = {10, 20, 30};
  EXPECT_EQ(joinStrings(numbers, ", ", true), "\"10\", \"20\", \"30\"");
}

TEST(JoinStringsTest, WithQuotesCustomType) {
  std::vector<CustomType> items = {{42}, {99}};
  EXPECT_EQ(joinStrings(items, " | ", true), "\"Custom(42)\" | \"Custom(99)\"");
}