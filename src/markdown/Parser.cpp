#include "Parser.hpp"
#include "markdown/AST.hpp"
#include "markdown/Lexer.hpp"
#include <algorithm>
#include <functional>
#include <memory>

namespace {

std::string PYTHON = "python";
std::string CPP = "c++";
std::string C_LANG = "c";
std::string JAVA = "java";
std::string JAVASCRIPT = "javascript";
std::string LUA = "lua";
std::string RUST = "rust";
std::string RUBY = "ruby";
std::string C_SHARP = "c#";
std::string HTML = "html";
std::string BASH = "bash";
std::string CSS = "css";
std::string NONE = "";

} // namespace

namespace QuickNotes::Markdown {

// --- Public Methods ---

// Constructor
///
Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_pos(0),
      m_rules({
          {TokenType::HEADING_1, [this] { return parseHeading(); }},
          {TokenType::HEADING_2, [this] { return parseHeading(); }},
          {TokenType::HEADING_3, [this] { return parseHeading(); }},
          {TokenType::HEADING_4, [this] { return parseHeading(); }},
          {TokenType::HEADING_5, [this] { return parseHeading(); }},
          {TokenType::HEADING_6, [this] { return parseHeading(); }},
          {TokenType::HORIZONTAL_RULE,
           [this] { return parseHorizontalRule(); }},
          {TokenType::BLOCK_QUOTE, [this] { return parseBlockQuote(); }},
          {TokenType::LIST_BULLET, [this] { return parseList(); }},
          {TokenType::LIST_ORDERED, [this] { return parseList(); }},
          {TokenType::CODE_FENCE, [this] { return parseCodeBlock(); }},
          {TokenType::TABLE_PIPE, [this] { return parseTable(); }},
      }) {}

AST Parser::parse() {
  AST tree;
  tree.root = std::make_unique<DocumentNode>();
  dispatch(tree);
  return tree;
}

// --- Private Methods ---

Token &Parser::peek() { return m_tokens[m_pos]; }

Token &Parser::advance() { return m_tokens[m_pos++]; }

bool Parser::isAtEnd() { return m_pos >= m_tokens.size(); }

void Parser::dispatch(AST &tree) {
  while (!isAtEnd()) {
    TokenType current = peek().type;
    if (current == TokenType::NEWLINE || current == TokenType::BLANK_LINE ||
        current == TokenType::END_OF_FILE) {
      advance();
      continue;
    }
    auto rule = std::ranges::find_if(m_rules, [&](const auto &r) {
      return r.first == current;
    });
    if (rule != m_rules.end()) {
      tree.root->children.push_back(rule->second());
    } else {
      tree.root->children.push_back(parseParagraph());
    }
  }
}

int Parser::headingLevel(TokenType type) {
  switch (type) {
    case TokenType::HEADING_1:
      return 1;
    case TokenType::HEADING_2:
      return 2;
    case TokenType::HEADING_3:
      return 3;
    case TokenType::HEADING_4:
      return 4;
    case TokenType::HEADING_5:
      return 5;
    case TokenType::HEADING_6:
      return 6;
    default:
      return 1;
  }
}

CodeLanguage Parser::language(std::string lang) {
  if (lang == PYTHON) {
    return CodeLanguage::PYTHON;
  }
  if (lang == CPP) {
    return CodeLanguage::CPP;
  }
  if (lang == C_LANG) {
    return CodeLanguage::C;
  }
  if (lang == JAVA) {
    return CodeLanguage::JAVA;
  }
  if (lang == JAVASCRIPT) {
    return CodeLanguage::JAVASCRIPT;
  }
  if (lang == LUA) {
    return CodeLanguage::LUA;
  }
  if (lang == RUST) {
    return CodeLanguage::RUST;
  }
  if (lang == RUBY) {
    return CodeLanguage::RUBY;
  }
  if (lang == C_SHARP) {
    return CodeLanguage::C_SHARP;
  }
  if (lang == HTML) {
    return CodeLanguage::HTML;
  }
  if (lang == BASH) {
    return CodeLanguage::BASH;
  }
  if (lang == CSS) {
    return CodeLanguage::CSS;
  }
  return CodeLanguage::NONE;
}

std::unique_ptr<HeadingNode> Parser::parseHeading() {
  Token &token = advance();
  auto node = std::make_unique<HeadingNode>();
  node->level = headingLevel(token.type);
  parseInLine(node->children);
  return node;
};

std::unique_ptr<HorizontalRuleNode> Parser::parseHorizontalRule() {
  return parseLeafBlock<HorizontalRuleNode>();
}

std::unique_ptr<BlockQuoteNode> Parser::parseBlockQuote() {
  return parseSimpleBlock<BlockQuoteNode>();
}

std::unique_ptr<ListNode> Parser::parseList() {
  Token &first = advance();
  ListKind kind = (first.type == TokenType::LIST_BULLET) ? ListKind::UNORDERED
                                                         : ListKind::ORDERED;
  auto node = std::make_unique<ListNode>(kind);
  node->items.push_back(parseListItem(first));
  auto isListItem = [&] {
    return !isAtEnd() && (peek().type == TokenType::LIST_BULLET ||
                          peek().type == TokenType::LIST_ORDERED);
  };
  while (isListItem()) {
    Token &token = advance();
    node->items.push_back(parseListItem(token));
  }
  return node;
}

std::unique_ptr<CodeBlockNode> Parser::parseCodeBlock() {
  advance();
  auto node = std::make_unique<CodeBlockNode>();
  if (!isAtEnd() && peek().type == TokenType::CODE_FENCE_LANG) {
    node->language = language(advance().value);
  }
  if (!isAtEnd() && peek().type == TokenType::TEXT) {
    node->children.push_back(std::make_unique<TextNode>(advance().value));
  }
  return node;
}

std::unique_ptr<TableNode> Parser::parseTable() {
  auto node = std::make_unique<TableNode>();
  node->rows.push_back(parseTableRow(true));
  if (!isAtEnd() && peek().type == TokenType::NEWLINE) advance();
  node->columnAlignments = parseTableAlignments();
  if (!isAtEnd() && peek().type == TokenType::NEWLINE) advance();
  while (!isAtEnd() && peek().type == TokenType::TABLE_PIPE) {
    node->rows.push_back(parseTableRow(false));
    if (!isAtEnd() && peek().type == TokenType::NEWLINE) advance();
  }
  return node;
}

std::unique_ptr<TableRowNode> Parser::parseTableRow(bool isHeader) {
  auto row = std::make_unique<TableRowNode>();
  row->isHeader = isHeader;

  while (!isAtEnd() && peek().type == TokenType::TABLE_PIPE) {
    advance();
    if (!isAtEnd() && peek().type == TokenType::TEXT) {
      row->cells.push_back(parseTableCell());
    }
  }

  return row;
}

std::unique_ptr<TableCellNode> Parser::parseTableCell() {
  auto cell = std::make_unique<TableCellNode>();
  Token &token = advance();
  cell->children.push_back(std::make_unique<TextNode>(token.value));
  return cell;
}

std::vector<ColumnAlignment> Parser::parseTableAlignments() {
  std::vector<ColumnAlignment> alignments;
  while (!isAtEnd() && peek().type == TokenType::TABLE_PIPE) {
    advance(); // consume TABLE_PIPE
    if (!isAtEnd() && peek().type == TokenType::TABLE_DELIMITER) {
      alignments.push_back(parseAlignment(advance().value));
    }
  }
  return alignments;
}

ColumnAlignment Parser::parseAlignment(const std::string &delimiter) {
  bool left = delimiter.front() == ':';
  bool right = delimiter.back() == ':';
  if (left && right) return ColumnAlignment::CENTER;
  if (left) return ColumnAlignment::LEFT;
  if (right) return ColumnAlignment::RIGHT;
  return ColumnAlignment::NONE;
}

std::unique_ptr<BoldNode> Parser::parseBold() {
  return parseEmphasis<BoldNode>();
}

std::unique_ptr<ItalicNode> Parser::parseItalic() {
  return parseEmphasis<ItalicNode>();
}

std::unique_ptr<ItalicBoldNode> Parser::parseItalicBold() {
  return parseEmphasis<ItalicBoldNode>();
}

std::unique_ptr<ParagraphNode> Parser::parseParagraph() {
  auto node = std::make_unique<ParagraphNode>();
  parseInLine(node->children);
  return node;
}

std::unique_ptr<ListItemNode> Parser::parseListItem(Token &token) {
  auto node = std::make_unique<ListItemNode>();
  parseInLine(node->children);
  return node;
}

std::unique_ptr<InLineCodeNode> Parser::parseInLineCode() {
  return std::make_unique<InLineCodeNode>(advance().value);
}

void Parser::parseInLine(std::vector<std::unique_ptr<Node>> &children) {
  while (!isAtEnd()) {
    TokenType type = peek().type;
    if (type == TokenType::BLANK_LINE || type == TokenType::END_OF_FILE ||
        type == TokenType::NEWLINE) {
      advance();
      break;
    }
    switch (type) {
      case TokenType::TEXT:
        children.push_back(std::make_unique<TextNode>(advance().value));
        break;
      case TokenType::BOLD:
        children.push_back(parseBold());
        break;
      case TokenType::ITALIC:
        children.push_back(parseItalic());
        break;
      case TokenType::BOLD_ITALIC:
        children.push_back(parseItalicBold());
        break;
      case TokenType::INLINE_CODE:
        children.push_back(parseInLineCode());
        break;
      default:
        advance();
        break;
    }
  }
}

} // namespace QuickNotes::Markdown
