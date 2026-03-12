#pragma once

#include "AST.hpp"
#include "Lexer.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace QuickNotes::Markdown {

/// @brief Parses a flat token stream into a Markdown AST.
///
/// Implements a recursive descent parser that consumes tokens produced
/// by Lexer and builds a tree of AST nodes rooted at DocumentNode.
///
/// Parsing is driven by a dispatch table (m_rules) that maps block-level
/// token types to their corresponding parse functions. Tokens not found
/// in the table are treated as the start of a paragraph.
///
/// Block-level constructs parsed:
/// - HEADING_1-6     → HeadingNode
/// - HORIZONTAL_RULE → HorizontalRuleNode
/// - BLOCK_QUOTE     → BlockQuoteNode
/// - LIST_BULLET     → ListNode (UNORDERED)
/// - LIST_ORDERED    → ListNode (ORDERED)
/// - CODE_FENCE      → CodeBlockNode
/// - TABLE_PIPE      → TableNode
/// - TEXT (fallback) → ParagraphNode
///
/// Inline constructs parsed within block content:
/// - TEXT            → TextNode
/// - BOLD            → BoldNode
/// - ITALIC          → ItalicNode
/// - BOLD_ITALIC     → ItalicBoldNode
/// - INLINE_CODE     → InLineCodeNode
///
/// NEWLINE and BLANK_LINE tokens are consumed as block separators and
/// do not produce AST nodes. Parsing terminates on END_OF_FILE.
///
/// @see Lexer
/// @see AST
/// @see IVisitor
class Parser {

  public:
    /// @brief Constructs a Parser.
    /// @param tokens vector of Tokens produced by the Lexer.
    explicit Parser(std::vector<Token> tokens);

    /// @brief Parse the vector of tokens and produce a abstract syntax tree.
    /// @return An abstract syntax tree.
    /// @see AST
    AST parse();

  private:
    using ParseFunction = std::function<NodePtr()>;
    using DispatchTable = std::vector<std::pair<TokenType, ParseFunction>>;

    std::vector<Token> m_tokens;
    size_t m_pos = 0;
    DispatchTable m_rules;

    /// @brief Get a reference to the token at current position of parser.
    /// @return a reference of to an instance of Token.
    Token &peek();

    /// @brief Get a reference to the token at current position of parser and
    /// increment position.
    /// @return a reference of to an instance of Token.
    Token &advance();

    /// @brief Check if parser has reached end of token list.
    /// @return true if the current position is at end tokens vector.
    bool isAtEnd();

    /// @brief Begin building the tree. Determines which methods to dispatch to.
    /// @param tree the AST to build.
    void dispatch(AST &tree);

    /// @brief Determine level of heading
    /// @return integer between 1-6.
    int headingLevel(TokenType type);

    /// @brief Determine language for code block token.
    /// @param lang string of the language's name.
    /// @return instance of CodeLanguage enum.
    CodeLanguage language(std::string lang);

    /// @brief Parse a heading token.
    /// @return an instance of HeadingNode.
    std::unique_ptr<HeadingNode> parseHeading();

    /// @brief Parse a code block token.
    /// @return an instance of CodeBlockNode.
    std::unique_ptr<CodeBlockNode> parseCodeBlock();

    /// @brief Parse a block quote token.
    /// @return an instance of BlockQuoteNode.
    std::unique_ptr<BlockQuoteNode> parseBlockQuote();

    /// @brief Parse a list token.
    /// @return an instance of ListNode.
    std::unique_ptr<ListNode> parseList();

    /// @brief Parse item of a list.
    /// @return an instance of a listItemNode.
    std::unique_ptr<ListItemNode> parseListItem(Token &token);

    /// @brief Parse a table token.
    /// @return an instance of TableNode.
    std::unique_ptr<TableNode> parseTable();

    /// @brief Parse a horizontal rule token.
    /// @return an instance of HorizontalRuleNode.
    std::unique_ptr<HorizontalRuleNode> parseHorizontalRule();

    /// @brief Parse a bold token.
    /// @return a pointer to a BoldNode.
    std::unique_ptr<BoldNode> parseBold();

    /// @brief Parse an italic token.
    /// @return a pointer to a ItalicNode.
    std::unique_ptr<ItalicNode> parseItalic();

    /// @brief Parse an italic-bold token.
    /// @return a pointer to a ItalicBoldNode.
    std::unique_ptr<ItalicBoldNode> parseItalicBold();

    /// @brief Parse in line code.
    /// @return a pointer to a InLineCodeNode.
    std::unique_ptr<InLineCodeNode> parseInLineCode();

    /// @brief Parse a paragraph.
    /// @return a pointer to a ParagraphNode.
    std::unique_ptr<ParagraphNode> parseParagraph();

    /// @brief Parse a table row token.
    /// @return an instance of TableRowNode.
    std::unique_ptr<TableRowNode> parseTableRow(bool isHeader);

    /// @brief Parse a table cell token.
    /// @return an instance of TableCellNode.
    std::unique_ptr<TableCellNode> parseTableCell();

    /// @brief Parse table alignments
    /// @return an instance of ColumnAlignment.
    std::vector<ColumnAlignment> parseTableAlignments();

    /// @brief parse tokens which could contained in a single line.
    ///
    /// @param children a reference to the children of a node (i.e. a vector of
    /// node pointers).
    void parseInLine(std::vector<std::unique_ptr<Node>> &children);

    /// @brief Parse an alignment token.
    /// @return an instance of ColumnAlignment.
    ColumnAlignment parseAlignment(const std::string &delimiter);

    template <typename T> std::unique_ptr<T> parseSimpleBlock() {
      Token &token = advance();
      auto node = std::make_unique<T>();
      parseInLine(node->children);
      return node;
    }

    template <typename T> std::unique_ptr<T> parseLeafBlock() {
      Token &token = advance();
      auto node = std::make_unique<T>();
      return node;
    }

    template <typename T> std::unique_ptr<T> parseEmphasis() {
      advance(); // consume opening BOLD
      auto node = std::make_unique<T>();
      if (!isAtEnd() && peek().type == TokenType::TEXT) {
        node->children.push_back(std::make_unique<TextNode>(advance().value));
      }
      auto isEmphasis = [&] {
        return !isAtEnd() && (peek().type == TokenType::BOLD ||
                              peek().type == TokenType::ITALIC ||
                              peek().type == TokenType::BOLD_ITALIC);
      };
      if (isEmphasis()) {
        advance();
      }
      return node;
    }
};
} // namespace QuickNotes::Markdown
