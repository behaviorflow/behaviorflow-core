// Copyright (c) 2025, Mitch Adams

#include <gtest/gtest.h>

#include "testing_utils.h"

using namespace bflow;
using namespace bflow::test;

TEST(ScopedTempFile, CleansUpFile) {
  std::string temp_path;
  {
    ScopedTempFile temp_file("gibberish\n");
    temp_path = temp_file.path_string();
    EXPECT_TRUE(std::filesystem::exists(temp_path));
  }
  // After temp_file is destroyed, file should not exist
  EXPECT_FALSE(std::filesystem::exists(temp_path));
}