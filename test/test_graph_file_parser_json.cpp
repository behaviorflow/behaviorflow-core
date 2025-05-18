// Copyright (c) 2024, Mitch Adams

#include <gtest/gtest.h>

#include "graph_file_parser_json.h"

using namespace bflow;

class GraphFileParserJsonTest : public ::testing::Test {
 protected:
  GraphFileParserJson parser;
  void SetUp() override { parser = GraphFileParserJson(); }
};

TEST_F(GraphFileParserJsonTest, EmptyString) {}

TEST_F(GraphFileParserJsonTest, EmptyFile) {}