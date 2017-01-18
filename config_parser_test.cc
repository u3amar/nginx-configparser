#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigStatementTest, SimpleConfigStatement) {
  NginxConfigStatement statement;
  statement.tokens_.push_back("alpha");
  statement.tokens_.push_back("go");

  std::string serialized_statement = statement.ToString(0);
  EXPECT_EQ(serialized_statement, "alpha go;\n");
}

class NginxConfigParserTest : public ::testing::Test {
protected:
  NginxConfigParser parser_;
  NginxConfig config_;

  bool ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    return parser_.Parse(&config_stream, &config_);
  }
};

TEST_F(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, MultilineConfig) {
  std::string serialized_config = 
    "port 80;\n gzip true;\n\n\n\n http-stream false;\n\n ";
  std::string simplified_config = 
    "port 80;\ngzip true;\nhttp-stream false;\n";

  ASSERT_TRUE(ParseString(serialized_config));
  EXPECT_EQ(config_.ToString(0), simplified_config);
}

TEST_F(NginxConfigParserTest, BadLineEndingConfig) {
  std::string serialized_config =
    "port 80;;\n";

  ASSERT_FALSE(ParseString(serialized_config));
}

TEST_F(NginxConfigParserTest, LeftUnbalancedBraceConfig) {
  std::string serialized_config =
    "port { 80";

  ASSERT_FALSE(ParseString(serialized_config));
}

TEST_F(NginxConfigParserTest, RightUnbalancedBraceConfig) {
  std::string serialized_config = 
    "port 80; }";
  
  ASSERT_FALSE(ParseString(serialized_config));
}

TEST_F(NginxConfigParserTest, BalancedBraceConfig) {
  std::string serialized_config = 
    "port { num 80; } dom { test 30; }";
  
  ASSERT_TRUE(ParseString(serialized_config));
}

TEST_F(NginxConfigParserTest, UnbalancedBraceConfig) {
  std::string serialized_config = 
    "port { num 80; } dom { test 30; }}";
  
  ASSERT_FALSE(ParseString(serialized_config));
}
