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

TEST(UnorderedListTest, DashSpaceIsListBullet) {
  auto tokens = lex("- item");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::LIST_BULLET);
}

TEST(UnorderedListTest, BulletFollowedByText) {
  auto tokens = lex("- item");
  ASSERT_GE(tokens.size(), 2u);
  EXPECT_EQ(tokens[0].type, TokenType::LIST_BULLET);
  EXPECT_EQ(tokens[1].type, TokenType::TEXT);
  EXPECT_EQ(tokens[1].value, "item");
}

TEST(UnorderedListTest, DashWithoutSpaceIsPlainText) {
  auto tokens = lex("-nospace");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::TEXT);
}

TEST(UnorderedListTest, MultipleBulletsProduceMultipleTokens) {
  auto tokens = lex("- one\n- two\n- three");
  auto t = types(tokens);
  EXPECT_EQ(std::count(t.begin(), t.end(), TokenType::LIST_BULLET), 3);
}

// --- Ordered List ---

TEST(OrderedListTest, NumberDotSpaceIsListOrdered) {
  auto tokens = lex("1. item");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::LIST_ORDERED);
}

TEST(OrderedListTest, OrderedListTokenCarriesNumber) {
  auto tokens = lex("1. item");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].value, "1");
}

TEST(OrderedListTest, MultiDigitNumberIsPreserved) {
  auto tokens = lex("42. item");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::LIST_ORDERED);
  EXPECT_EQ(tokens[0].value, "42");
}

TEST(OrderedListTest, NumberDotWithoutSpaceIsPlainText) {
  auto tokens = lex("1.nospace");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::TEXT);
}

TEST(OrderedListTest, MultipleOrderedItemsProduceMultipleTokens) {
  auto tokens = lex("1. one\n2. two\n3. three");
  auto t = types(tokens);
  EXPECT_EQ(std::count(t.begin(), t.end(), TokenType::LIST_ORDERED), 3);
}

// --- Code Blocks ---

TEST(CodeBlockTest, TripleBacktickProducesCodeFence) {
  auto tokens = lex("```\ncode\n```");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::CODE_FENCE), t.end());
}

TEST(CodeBlockTest, CodeFenceWithLanguageProducesLangToken) {
  auto tokens = lex("```cpp\ncode\n```");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::CODE_FENCE_LANG), t.end());
}

TEST(CodeBlockTest, CodeFenceLangTokenCarriesLanguageName) {
  auto tokens = lex("```cpp\ncode\n```");
  auto it = std::find_if(tokens.begin(), tokens.end(), [](const Token &tok) {
    return tok.type == TokenType::CODE_FENCE_LANG;
  });
  ASSERT_NE(it, tokens.end());
  EXPECT_EQ(it->value, "cpp");
}

TEST(CodeBlockTest, CodeBlockContentProducesTextToken) {
  auto tokens = lex("```\nhello world\n```");
  auto it = std::find_if(tokens.begin(), tokens.end(), [](const Token &tok) {
    return tok.type == TokenType::TEXT;
  });
  ASSERT_NE(it, tokens.end());
  EXPECT_EQ(it->value, "hello world");
}

TEST(CodeBlockTest, InlineCodeProducesInlineCodeToken) {
  auto tokens = lex("`code`");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::INLINE_CODE);
}

TEST(CodeBlockTest, InlineCodeCarriesValue) {
  auto tokens = lex("`some_func()`");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].value, "some_func()");
}

// --- Tables ---

TEST(TableTest, PipeProducesTablePipe) {
  auto tokens = lex("| cell |");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::TABLE_PIPE), t.end());
}

TEST(TableTest, TableRowProducesTextToken) {
  auto tokens = lex("| hello |");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::TEXT), t.end());
  auto it = std::find_if(tokens.begin(), tokens.end(), [](const Token &tok) {
    return tok.type == TokenType::TEXT;
  });
  EXPECT_EQ(it->value, "hello");
}

TEST(TableTest, DelimiterRowProducesTableDelimiterToken) {
  auto tokens = lex("| col |\n| --- |");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::TABLE_DELIMITER), t.end());
}

TEST(TableTest, MultiColumnRowProducesMultiplePipes) {
  auto tokens = lex("| a | b | c |");
  auto t = types(tokens);
  EXPECT_GE(std::count(t.begin(), t.end(), TokenType::TABLE_PIPE), 3);
}

// --- Plain Text ---

TEST(PlainTextTest, BasicTextProducesTextToken) {
  auto tokens = lex("hello world");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::TEXT);
  EXPECT_EQ(tokens[0].value, "hello world");
}

TEST(PlainTextTest, EmphasisInnerTextIsTextToken) {
  auto tokens = lex("**bold**");
  ASSERT_GE(tokens.size(), 2u);
  EXPECT_EQ(tokens[1].type, TokenType::TEXT);
  EXPECT_EQ(tokens[1].value, "bold");
}

TEST(PlainTextTest, TextWithSpecialCharsIsPreserved) {
  auto tokens = lex("hello, world! (2024)");
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].type, TokenType::TEXT);
  EXPECT_EQ(tokens[0].value, "hello, world! (2024)");
}

// --- Line / Column Tracking ---

class LineColumnTest : public ::testing::Test {};

TEST_F(LineColumnTest, FirstTokenIsOnLineOne) {
  Lexer lexer("hello");
  auto tokens = lexer.tokenize();
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].line, 1);
}

TEST_F(LineColumnTest, FirstTokenStartsAtColumnOne) {
  Lexer lexer("hello");
  auto tokens = lexer.tokenize();
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens[0].column, 1);
}

TEST_F(LineColumnTest, TokenAfterNewlineIsOnNextLine) {
  Lexer lexer("first\nsecond");
  auto tokens = lexer.tokenize();
  // find the TEXT token with value "second"
  auto it = std::find_if(tokens.begin(), tokens.end(), [](const Token &tok) {
    return tok.type == TokenType::TEXT && tok.value == "second";
  });
  ASSERT_NE(it, tokens.end());
  EXPECT_EQ(it->line, 2);
}

// --- Multi-element Documents ---

TEST(MultiElementTest, HeadingFollowedByParagraph) {
  auto tokens = lex("# Title\n\nsome text");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::HEADING_1), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::BLANK_LINE), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::TEXT), t.end());
}

TEST(MultiElementTest, ListAfterParagraphProducesBothTypes) {
  auto tokens = lex("Some intro.\n\n- item one\n- item two");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::TEXT), t.end());
  EXPECT_EQ(std::count(t.begin(), t.end(), TokenType::LIST_BULLET), 2);
}

TEST(MultiElementTest, MixedBlocksProduceCorrectTokenSequence) {
  auto tokens = lex("# H1\n\n---\n\n> quote");
  auto t = types(tokens);
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::HEADING_1), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::HORIZONTAL_RULE), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::BLOCK_QUOTE), t.end());
}

TEST(MultiElementTest, FullDocument) {
  const std::string doc =
      "# Heading\n"
      "\n"
      "Some **bold** and *italic* text with `inline code`.\n"
      "\n"
      "> A block quote.\n"
      "\n"
      "- item one\n"
      "- item two\n"
      "\n"
      "1. first\n"
      "2. second\n"
      "\n"
      "---\n"
      "\n"
      "```cpp\n"
      "int x = 0;\n"
      "```\n"
      "\n"
      "| Name  | Age |\n"
      "| :---- | --: |\n"
      "| Alice | 30  |\n";

  auto t = types(lex(doc));

  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::HEADING_1), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::BOLD), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::ITALIC), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::INLINE_CODE), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::BLOCK_QUOTE), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::LIST_BULLET), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::LIST_ORDERED), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::HORIZONTAL_RULE), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::CODE_FENCE), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::CODE_FENCE_LANG), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::TABLE_PIPE), t.end());
  EXPECT_NE(std::find(t.begin(), t.end(), TokenType::TABLE_DELIMITER), t.end());
}
