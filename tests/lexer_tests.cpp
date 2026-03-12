#include "markdown/Lexer.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace QuickNotes::Markdown;

static std::vector<Token> lex(const std::string &src) {
  Lexer lexer(src);
  auto tokens = lexer.tokenize();
  if (!tokens.empty() && tokens.back().type == TokenType::END_OF_FILE) {
    tokens.pop_back();
  }
  return tokens;
}

static std::vector<TokenType> types(const std::vector<Token> &tokens) {
  std::vector<TokenType> result;
  result.reserve(tokens.size());
  for (const auto &t : tokens)
    result.push_back(t.type);
  return result;
}

// --- End of File ---

TEST(LexerEoF, EmptyStringYieldsOnlyEoF) {
  Lexer lexer("");
  auto tokens = lexer.tokenize();
  ASSERT_EQ(tokens.size(), 1u);
  EXPECT_EQ(tokens[0].type, TokenType::END_OF_FILE);
}

// --- Heading Tests ---

class HeadingTest : public ::testing::Test {};

TEST_F(HeadingTest, H1) {
  auto tokens = lex("# hello");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::HEADING_1);
}

TEST_F(HeadingTest, H2) {
  EXPECT_EQ(lex("## Hi")[0].type, TokenType::HEADING_2);
}

TEST_F(HeadingTest, H3) {
  EXPECT_EQ(lex("### Hi")[0].type, TokenType::HEADING_3);
}

TEST_F(HeadingTest, H4) {
  EXPECT_EQ(lex("#### Hi")[0].type, TokenType::HEADING_4);
}

TEST_F(HeadingTest, H5) {
  EXPECT_EQ(lex("##### Hi")[0].type, TokenType::HEADING_5);
}

TEST_F(HeadingTest, H6) {
  EXPECT_EQ(lex("###### Hi")[0].type, TokenType::HEADING_6);
}

TEST_F(HeadingTest, SevenPoundsISPlainText) {
  auto tokens = lex("####### hi");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::TEXT);
}

TEST_F(HeadingTest, PoundWithoutSpaceIsPlainText) {
  auto tokens = lex("#NoSpace");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::TEXT);
}

// --- New Lines and Blank Lines ---

TEST(NewLineTest, SingleNewLineProducesNewLineToken) {
  auto tokens = lex("a\nb");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::NEWLINE), t.end());
}

TEST(NewLineTest, DoubleNewLineProducesBlankLine) {
  auto tokens = lex("a\n\nb");
  auto t = types(tokens);
  EXPECT_EQ(std::count(t.begin(), t.end(), TokenType::BLANK_LINE), 1);
}

TEST(NewLineTest, ManyConsecutiveNewLinesProduceOneBlankLine) {
  auto tokens = lex("a\n\n\n\nb");
  auto t = types(tokens);
  EXPECT_EQ(std::count(t.begin(), t.end(), TokenType::BLANK_LINE), 1);
}

// --- Block Quotes ---

TEST(BlockQuoteTest, BasicBlockQuote) {
  auto tokens = lex("> some text");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::BLOCK_QUOTE);
}

TEST(BlockQuoteTest, BlockQuoteFollowedByText) {
  auto tokens = lex("> quote");
  ASSERT_GE(tokens.size(), 2u);
  EXPECT_EQ(tokens[0].type, TokenType::BLOCK_QUOTE);
  EXPECT_EQ(tokens[1].type, TokenType::TEXT);
  EXPECT_EQ(tokens[1].value, "quote");
}

// --- Emphasis: italic, bold, bold-italic ---

TEST(EmphasisTest, SingleAsterixIsItalic) {
  auto tokens = lex("*text*");
  EXPECT_EQ(tokens[0].type, TokenType::ITALIC);
}

TEST(EmphasisTest, DoubleAsterixIsBold) {
  auto tokens = lex("**text**");
  EXPECT_EQ(tokens[0].type, TokenType::BOLD);
}

TEST(EmphasisTest, TripleAsterixIsBoldItalic) {
  auto tokens = lex("***text***");
  EXPECT_EQ(tokens[0].type, TokenType::BOLD_ITALIC);
}

// --- Horizontal Rule ---

TEST(HoriontalRuleTest, ThreeDashesProducesRule) {
  auto tokens = lex("---");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::HORIZONTAL_RULE), t.end());
}

TEST(HoriontalRuleTest, ManyDashesProducesRule) {
  auto tokens = lex("------------");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::HORIZONTAL_RULE), t.end());
}

// --- Unordered List ---

// --- Ordered List ---

// --- Code Blocks ---

// --- Tables ---

// --- Plain Text ---

// --- Line / Column tracking ---

// --- Multi-element documents ---
