#include "markdown/AST.hpp"
#include "markdown/Lexer.hpp"
#include "markdown/Parser.hpp"
#include <gtest/gtest.h>

using namespace QuickNotes::Markdown;

// --- Helpers ---

static AST parse(const std::string &src) {
  Lexer lexer(src);
  return Parser(lexer.tokenize()).parse();
}

template <typename T> const T *as(const Node *node) {
  return dynamic_cast<const T *>(node);
}

// --- Document ---

TEST(DocumentTest, EmptySourceProducesEmptyDocument) {
  auto ast = parse("");
  ASSERT_NE(ast.root, nullptr);
  EXPECT_TRUE(ast.root->children.empty());
}

// --- Headings ---

class HeadingTest : public ::testing::Test {};

TEST_F(HeadingTest, H1HasLevelOne) {
  auto ast = parse("# Hello");
  ASSERT_EQ(ast.root->children.size(), 1u);
  auto *node = as<HeadingNode>(ast.root->children[0].get());
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->level, 1);
}

TEST_F(HeadingTest, H3HasLevelThree) {
  auto ast = parse("### Hello");
  auto *node = as<HeadingNode>(ast.root->children[0].get());
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->level, 3);
}

TEST_F(HeadingTest, H6HasLevelSix) {
  auto ast = parse("###### Hello");
  auto *node = as<HeadingNode>(ast.root->children[0].get());
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->level, 6);
}

TEST_F(HeadingTest, HeadingHasTextChild) {
  auto ast = parse("# Hello");
  auto *heading = as<HeadingNode>(ast.root->children[0].get());
  ASSERT_NE(heading, nullptr);
  ASSERT_FALSE(heading->children.empty());
  auto *text = as<TextNode>(heading->children[0].get());
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->value, "Hello");
}

// --- Paragraphs ---

class ParagraphTest : public ::testing::Test {};

TEST_F(ParagraphTest, PlainTextProducesParagraph) {
  auto ast = parse("hello world");
  ASSERT_EQ(ast.root->children.size(), 1u);
  EXPECT_NE(as<ParagraphNode>(ast.root->children[0].get()), nullptr);
}

TEST_F(ParagraphTest, ParagraphHasTextChild) {
  auto ast = parse("hello world");
  auto *para = as<ParagraphNode>(ast.root->children[0].get());
  ASSERT_NE(para, nullptr);
  ASSERT_FALSE(para->children.empty());
  auto *text = as<TextNode>(para->children[0].get());
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->value, "hello world");
}

TEST_F(ParagraphTest, BoldInParagraph) {
  auto ast = parse("**bold**");
  auto *para = as<ParagraphNode>(ast.root->children[0].get());
  ASSERT_NE(para, nullptr);
  ASSERT_FALSE(para->children.empty());
  EXPECT_NE(as<BoldNode>(para->children[0].get()), nullptr);
}

TEST_F(ParagraphTest, ItalicInParagraph) {
  auto ast = parse("*italic*");
  auto *para = as<ParagraphNode>(ast.root->children[0].get());
  ASSERT_NE(para, nullptr);
  EXPECT_NE(as<ItalicNode>(para->children[0].get()), nullptr);
}

TEST_F(ParagraphTest, BoldItalicInParagraph) {
  auto ast = parse("***bolditalic***");
  auto *para = as<ParagraphNode>(ast.root->children[0].get());
  ASSERT_NE(para, nullptr);
  EXPECT_NE(as<ItalicBoldNode>(para->children[0].get()), nullptr);
}

TEST_F(ParagraphTest, InlineCodeInParagraph) {
  auto ast = parse("`code`");
  auto *para = as<ParagraphNode>(ast.root->children[0].get());
  ASSERT_NE(para, nullptr);
  EXPECT_NE(as<InLineCodeNode>(para->children[0].get()), nullptr);
}

TEST_F(ParagraphTest, InlineCodeCarriesValue) {
  auto ast = parse("`my_func()`");
  auto *para = as<ParagraphNode>(ast.root->children[0].get());
  ASSERT_NE(para, nullptr);
  auto *code = as<InLineCodeNode>(para->children[0].get());
  ASSERT_NE(code, nullptr);
  EXPECT_EQ(code->value, "my_func()");
}

TEST_F(ParagraphTest, BoldNodeHasTextChild) {
  auto ast = parse("**bold**");
  auto *para = as<ParagraphNode>(ast.root->children[0].get());
  ASSERT_NE(para, nullptr);
  auto *bold = as<BoldNode>(para->children[0].get());
  ASSERT_NE(bold, nullptr);
  ASSERT_FALSE(bold->children.empty());
  auto *text = as<TextNode>(bold->children[0].get());
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->value, "bold");
}

// --- Horizontal Rule ---

TEST(HorizontalRuleTest, ProducesHorizontalRuleNode) {
  auto ast = parse("---");
  ASSERT_EQ(ast.root->children.size(), 1u);
  EXPECT_NE(as<HorizontalRuleNode>(ast.root->children[0].get()), nullptr);
}

// --- Block Quote ---

TEST(BlockQuoteTest, ProducesBlockQuoteNode) {
  auto ast = parse("> quote");
  ASSERT_EQ(ast.root->children.size(), 1u);
  EXPECT_NE(as<BlockQuoteNode>(ast.root->children[0].get()), nullptr);
}

TEST(BlockQuoteTest, BlockQuoteHasTextChild) {
  auto ast = parse("> quote");
  auto *bq = as<BlockQuoteNode>(ast.root->children[0].get());
  ASSERT_NE(bq, nullptr);
  ASSERT_FALSE(bq->children.empty());
  auto *text = as<TextNode>(bq->children[0].get());
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->value, "quote");
}

// --- Lists ---

class ListTest : public ::testing::Test {};

TEST_F(ListTest, UnorderedListHasCorrectKind) {
  auto ast = parse("- item");
  auto *list = as<ListNode>(ast.root->children[0].get());
  ASSERT_NE(list, nullptr);
  EXPECT_EQ(list->kind, ListKind::UNORDERED);
}

TEST_F(ListTest, OrderedListHasCorrectKind) {
  auto ast = parse("1. item");
  auto *list = as<ListNode>(ast.root->children[0].get());
  ASSERT_NE(list, nullptr);
  EXPECT_EQ(list->kind, ListKind::ORDERED);
}

TEST_F(ListTest, UnorderedListItemCount) {
  auto ast = parse("- one\n- two\n- three");
  auto *list = as<ListNode>(ast.root->children[0].get());
  ASSERT_NE(list, nullptr);
  EXPECT_EQ(list->items.size(), 3u);
}

TEST_F(ListTest, OrderedListItemCount) {
  auto ast = parse("1. one\n2. two");
  auto *list = as<ListNode>(ast.root->children[0].get());
  ASSERT_NE(list, nullptr);
  EXPECT_EQ(list->items.size(), 2u);
}

TEST_F(ListTest, ListItemHasTextChild) {
  auto ast = parse("- hello");
  auto *list = as<ListNode>(ast.root->children[0].get());
  ASSERT_NE(list, nullptr);
  auto *item = as<ListItemNode>(list->items[0].get());
  ASSERT_NE(item, nullptr);
  ASSERT_FALSE(item->children.empty());
  auto *text = as<TextNode>(item->children[0].get());
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->value, "hello");
}

// --- Code Blocks ---

class CodeBlockTest : public ::testing::Test {};

TEST_F(CodeBlockTest, ProducesCodeBlockNode) {
  auto ast = parse("```\ncode\n```");
  ASSERT_EQ(ast.root->children.size(), 1u);
  EXPECT_NE(as<CodeBlockNode>(ast.root->children[0].get()), nullptr);
}

TEST_F(CodeBlockTest, NoLanguageTagDefaultsToNone) {
  auto ast = parse("```\ncode\n```");
  auto *block = as<CodeBlockNode>(ast.root->children[0].get());
  ASSERT_NE(block, nullptr);
  EXPECT_EQ(block->language, CodeLanguage::NONE);
}

TEST_F(CodeBlockTest, LanguageTagParsedCorrectly) {
  auto ast = parse("```c++\ncode\n```");
  auto *block = as<CodeBlockNode>(ast.root->children[0].get());
  ASSERT_NE(block, nullptr);
  EXPECT_EQ(block->language, CodeLanguage::CPP);
}

TEST_F(CodeBlockTest, CodeBlockHasTextChild) {
  auto ast = parse("```\nhello world\n```");
  auto *block = as<CodeBlockNode>(ast.root->children[0].get());
  ASSERT_NE(block, nullptr);
  ASSERT_FALSE(block->children.empty());
  auto *text = as<TextNode>(block->children[0].get());
  ASSERT_NE(text, nullptr);
  EXPECT_EQ(text->value, "hello world");
}

// --- Tables ---

class TableTest : public ::testing::Test {};

TEST_F(TableTest, ProducesTableNode) {
  auto ast = parse("| col |\n| --- |\n| val |");
  ASSERT_EQ(ast.root->children.size(), 1u);
  EXPECT_NE(as<TableNode>(ast.root->children[0].get()), nullptr);
}

TEST_F(TableTest, FirstRowIsHeader) {
  auto ast = parse("| col |\n| --- |\n| val |");
  auto *table = as<TableNode>(ast.root->children[0].get());
  ASSERT_NE(table, nullptr);
  ASSERT_FALSE(table->rows.empty());
  auto *header = as<TableRowNode>(table->rows[0].get());
  ASSERT_NE(header, nullptr);
  EXPECT_TRUE(header->isHeader);
}

TEST_F(TableTest, DataRowIsNotHeader) {
  auto ast = parse("| col |\n| --- |\n| val |");
  auto *table = as<TableNode>(ast.root->children[0].get());
  ASSERT_NE(table, nullptr);
  ASSERT_GE(table->rows.size(), 2u);
  auto *row = as<TableRowNode>(table->rows[1].get());
  ASSERT_NE(row, nullptr);
  EXPECT_FALSE(row->isHeader);
}

TEST_F(TableTest, RowCount) {
  auto ast = parse("| a | b |\n| --- | --- |\n| 1 | 2 |\n| 3 | 4 |");
  auto *table = as<TableNode>(ast.root->children[0].get());
  ASSERT_NE(table, nullptr);
  // header row + 2 data rows (alignment row is consumed, not stored)
  EXPECT_EQ(table->rows.size(), 3u);
}

TEST_F(TableTest, LeftAlignmentParsed) {
  auto ast = parse("| col |\n| :--- |\n| val |");
  auto *table = as<TableNode>(ast.root->children[0].get());
  ASSERT_NE(table, nullptr);
  ASSERT_FALSE(table->columnAlignments.empty());
  EXPECT_EQ(table->columnAlignments[0], ColumnAlignment::LEFT);
}

TEST_F(TableTest, RightAlignmentParsed) {
  auto ast = parse("| col |\n| ---: |\n| val |");
  auto *table = as<TableNode>(ast.root->children[0].get());
  ASSERT_NE(table, nullptr);
  EXPECT_EQ(table->columnAlignments[0], ColumnAlignment::RIGHT);
}

TEST_F(TableTest, CenterAlignmentParsed) {
  auto ast = parse("| col |\n| :---: |\n| val |");
  auto *table = as<TableNode>(ast.root->children[0].get());
  ASSERT_NE(table, nullptr);
  EXPECT_EQ(table->columnAlignments[0], ColumnAlignment::CENTER);
}

// --- Multi-block documents ---

TEST(MultiBlockTest, HeadingFollowedByParagraph) {
  auto ast = parse("# Title\n\nsome text");
  ASSERT_EQ(ast.root->children.size(), 2u);
  EXPECT_NE(as<HeadingNode>(ast.root->children[0].get()), nullptr);
  EXPECT_NE(as<ParagraphNode>(ast.root->children[1].get()), nullptr);
}

TEST(MultiBlockTest, CorrectChildCountForMixedDocument) {
  auto ast = parse(
      "# Heading\n\n"
      "A paragraph.\n\n"
      "- item one\n"
      "- item two\n\n"
      "---\n"
  );
  // heading, paragraph, list, horizontal rule
  EXPECT_EQ(ast.root->children.size(), 4u);
}

TEST(MultiBlockTest, CorrectNodeTypesInOrder) {
  auto ast = parse(
      "# Heading\n\n"
      "A paragraph.\n\n"
      "- item one\n"
      "- item two\n\n"
      "---\n"
  );
  EXPECT_NE(as<HeadingNode>(ast.root->children[0].get()), nullptr);
  EXPECT_NE(as<ParagraphNode>(ast.root->children[1].get()), nullptr);
  EXPECT_NE(as<ListNode>(ast.root->children[2].get()), nullptr);
  EXPECT_NE(as<HorizontalRuleNode>(ast.root->children[3].get()), nullptr);
}
