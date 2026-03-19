#include "Lexer.hpp"
#include <cctype>
#include <cstddef>
#include <functional>
#include <stdexcept>

namespace {
constexpr char NULL_CHAR = '\0';
constexpr char NEW_LINE_CHAR = '\n';
constexpr char POUND_SIGN = '#';
constexpr char SPACE = ' ';
constexpr char TAB = '\t';
constexpr char GREATER_THAN = '>';
constexpr char COLON = ':';
constexpr char BACK_TICK = '`';
constexpr char ASTERIX = '*';
constexpr char PIPE = '|';
constexpr char DASH = '-';
constexpr int MAX_HEADER_DEPTH = 6;
constexpr int MAX_EMPHASIS_CHARACTERS = 3;
} // namespace

namespace QuickNotes::Markdown {

// Constructor
Lexer::Lexer(std::string source)
    : m_source(std::move(source)),
      m_position(0),
      m_line(1),
      m_column(1),
      m_state(LexerState::DEFAULT) {}

// -- Private Methods -- //

bool Lexer::isAtEnd() const { return m_position >= m_source.size(); }

char Lexer::peek() const {
  return isAtEnd() ? NULL_CHAR : m_source[m_position];
}

char Lexer::peekNext() const {
  return isAtEnd() ? NULL_CHAR : m_source[m_position + 1];
}

char Lexer::advance() {
  char c = m_source[m_position++];
  if (c == NEW_LINE_CHAR) {
    m_line++;
    m_column = 1;
  } else {
    m_column++;
  }
  return c;
}

Token Lexer::makeToken(TokenType type, std::string value) {
  return Token{type, std::move(value), m_line, m_column};
}

Token Lexer::makeToken(TokenType type, int line, int col, std::string value) {
  return Token{type, std::move(value), line, col};
}

using TokenList = std::vector<Token>;
using TokenListRef = std::vector<Token> &;

void Lexer::handleState(TokenListRef tokens) {
  switch (m_state) {
    case LexerState::DEFAULT: handleDefaultState(tokens); break;
    case LexerState::IN_CODE_BLOCK: handleCodeBlockState(tokens); break;
    case LexerState::IN_TABLE: handleTableState(tokens); break;
    default:
      // I am pretty sure this actually impossible to reach.
      throw std::runtime_error("Unknown lexer state");
  }
}

void Lexer::handleDefaultState(TokenListRef tokens) {
  char c = peek();
  switch (c) {
    case NEW_LINE_CHAR: readNewLine(tokens); break;
    case POUND_SIGN: readHeading(tokens); break;
    case GREATER_THAN: readBlockQuote(tokens); break;
    case BACK_TICK: readBackTick(tokens); break;
    case ASTERIX: readEmphasis(tokens); break;
    case PIPE: readPipe(tokens); break;
    case DASH: readDash(tokens); break;
    default:
      if (std::isdigit((unsigned char)c)) {
        readOrderedList(tokens);
      } else {
        readText(tokens);
      }
      break;
  }
}

void Lexer::handleCodeBlockState(TokenListRef tokens) {
  std::string code;
  auto isCodeBlockDelimiter = [&]() {
    return m_column == 1 && peek() == BACK_TICK && peekNext() == BACK_TICK &&
           m_position + 2 < m_source.size() &&
           m_source[m_position + 2] == BACK_TICK;
  };
  int line = m_line, col = m_column;
  while (!isAtEnd()) {
    if (isCodeBlockDelimiter()) {
      advance();
      advance();
      advance();
      while (peek() != NEW_LINE_CHAR && !isAtEnd()) {
        advance();
      }
      if (!code.empty() && code.back() == '\n') {
        code.pop_back();
      }
      tokens.push_back(makeToken(TokenType::TEXT, line, col, std::move(code)));
      tokens.push_back(makeToken(TokenType::CODE_FENCE, m_line, m_column));
      m_state = LexerState::DEFAULT;
      return;
    }
    code += advance();
  }
  if (!code.empty()) {
    tokens.push_back(makeToken(TokenType::TEXT, line, col, std::move(code)));
  }
  tokens.push_back(makeToken(TokenType::CODE_FENCE, line, col));
  m_state = LexerState::DEFAULT;
}

void Lexer::handleTableState(TokenListRef tokens) {
  while (peek() == SPACE || peek() == TAB)
    advance();
  char c = peek();

  using Predicate = std::function<bool(char)>;
  using Handler = std::function<void()>;
  using Rule = std::pair<Predicate, Handler>;

  // TODO: clean this mess up !
  const std::array<Rule, 4> rules = {
      {{[](char c) { return c == NULL_CHAR; },
        [&]() { m_state = LexerState::DEFAULT; }},
       {[](char c) { return c == NEW_LINE_CHAR; },
        [&]() {
          int line = m_line, col = m_column;
          advance();
          if (peek() == NEW_LINE_CHAR) {
            advance();
            tokens.push_back(makeToken(TokenType::BLANK_LINE, line, col));
            m_state = LexerState::DEFAULT;
            return;
          }

          tokens.push_back(makeToken(TokenType::NEWLINE, line, col));
          size_t savedPosition = m_position;
          int savedLine = m_line;
          int savedColumn = m_column;

          while (peek() == SPACE || peek() == TAB)
            advance();

          if (peek() != PIPE) {
            m_position = savedPosition;
            m_line = savedLine;
            m_column = savedColumn;
            m_state = LexerState::DEFAULT;
          }
        }},
       {[](char c) { return c == PIPE; },
        [&]() {
          int line = m_line, col = m_column;
          advance();
          tokens.push_back(makeToken(TokenType::TABLE_PIPE, line, col));
          while (peek() == SPACE || peek() == TAB) {
            advance();
          }
          if (peek() == NEW_LINE_CHAR || isAtEnd()) return;
          (peek() == DASH || peek() == COLON) ? readTableDelimiter(tokens) :
                                                readTableCell(tokens);
        }},
       {[](char) { return true; }, [&]() { readTableCell(tokens); }}}
  };
  for (auto &[predicate, handler] : rules) {
    if (predicate(c)) {
      handler();
      return;
    }
  }
}

void Lexer::readNewLine(TokenListRef tokens) {
  if (isAtEnd() || peek() == NULL_CHAR) return;
  int line = m_line, col = m_column;
  advance();
  if (peek() == NEW_LINE_CHAR) {
    while (peek() == NEW_LINE_CHAR) {
      advance();
    }
    tokens.push_back(makeToken(TokenType::BLANK_LINE, line, col));
    m_state = LexerState::DEFAULT;
    return;
  }
  tokens.push_back(makeToken(TokenType::NEWLINE, line, col));
}

void Lexer::readHeading(TokenListRef tokens) {
  int line = m_line, col = m_column;
  int level = 0;

  while (peek() == POUND_SIGN) {
    advance();
    level++;
  }

  if (level > MAX_HEADER_DEPTH || peek() != SPACE) {
    tokens.push_back(makeToken(TokenType::TEXT, line, col));
    m_state = LexerState::DEFAULT;
    return;
  }
  advance(); // consume the space !
  switch (level) {
    case 1: tokens.push_back(makeToken(TokenType::HEADING_1, line, col)); break;
    case 2: tokens.push_back(makeToken(TokenType::HEADING_2, line, col)); break;
    case 3: tokens.push_back(makeToken(TokenType::HEADING_3, line, col)); break;
    case 4: tokens.push_back(makeToken(TokenType::HEADING_4, line, col)); break;
    case 5: tokens.push_back(makeToken(TokenType::HEADING_5, line, col)); break;
    case 6: tokens.push_back(makeToken(TokenType::HEADING_6, line, col)); break;
  }
}

void Lexer::readBlockQuote(TokenListRef tokens) {
  tokens.push_back(makeToken(TokenType::BLOCK_QUOTE));
  advance();
  while (peek() == SPACE)
    advance();
}

void Lexer::readBackTick(TokenListRef tokens) {
  int line = m_line, col = m_column;

  if (peekNext() == BACK_TICK && m_position + 2 < m_source.size() &&
      m_source[m_position + 2] == BACK_TICK) {
    advance();
    advance();
    advance();
    tokens.push_back(makeToken(TokenType::CODE_FENCE, line, col));

    std::string lang;
    while (!isAtEnd() && peek() != NEW_LINE_CHAR) {
      lang += advance();
    }

    if (!isAtEnd() && peek() == NEW_LINE_CHAR) {
      advance();
    }

    if (!lang.empty()) {
      tokens.push_back(
          makeToken(TokenType::CODE_FENCE_LANG, line, col, std::move(lang))
      );
    }
    m_state = LexerState::IN_CODE_BLOCK;
    return;
  }

  advance();
  std::string code;
  while (!isAtEnd() && peek() != BACK_TICK && peek() != NEW_LINE_CHAR) {
    code += advance();
  }
  if (peek() == BACK_TICK) {
    advance();
  }
  tokens.push_back(
      makeToken(TokenType::INLINE_CODE, line, col, std::move(code))
  );
}

void Lexer::readEmphasis(TokenListRef tokens) {
  int line = m_line, col = m_column;
  int count = 0;
  while (peek() == ASTERIX) {
    advance();
    count++;
  }

  switch (count) {
    case 1: tokens.push_back(makeToken(TokenType::ITALIC, line, col)); break;
    case 2: tokens.push_back(makeToken(TokenType::BOLD, line, col)); break;
    case 3:
      tokens.push_back(makeToken(TokenType::BOLD_ITALIC, line, col));
      break;
  }
}

void Lexer::readPipe(TokenListRef tokens) {
  int line = m_line, col = m_column;
  advance();
  tokens.push_back(makeToken(TokenType::TABLE_PIPE, line, col));
  m_state = LexerState::IN_TABLE;
}

void Lexer::readDash(TokenListRef tokens) {

  int line = m_line, col = m_column;
  auto isHorizontalRule = [&]() {
    return peek() == DASH && peekNext() == DASH &&
           m_position + 2 < m_source.size() && m_source[m_position + 2] == DASH;
  };

  if (isHorizontalRule()) {
    do {
      advance();
    } while (peek() != NEW_LINE_CHAR && !isAtEnd());
    tokens.push_back(makeToken(TokenType::HORIZONTAL_RULE, line, col));
    tokens.push_back(makeToken(TokenType::NEWLINE, m_line, m_column));
    return;
  }

  if (peek() == DASH && peekNext() == SPACE) {
    advance();
    advance();
    tokens.push_back(makeToken(TokenType::LIST_BULLET, line, col));
    return;
  }

  readText(tokens);
}

void Lexer::readOrderedList(TokenListRef tokens) {
  int line = m_line, col = m_column;
  size_t savedPosition = m_position;
  int savedLine = m_line;
  int savedColumn = m_column;

  std::string number;
  while (std::isdigit((unsigned char)peek()))
    number += advance();

  if (peek() != '.' || peekNext() != SPACE) {
    m_position = savedPosition;
    m_line = savedLine;
    m_column = savedColumn;
    readText(tokens);
    return;
  }

  advance();
  advance();

  tokens.push_back(
      makeToken(TokenType::LIST_ORDERED, line, col, std::move(number))
  );
}

void Lexer::readText(TokenListRef tokens) {
  int line = m_line, col = m_column;
  std::string text;

  while (!isAtEnd()) {
    char c = peek();

    if (c == NEW_LINE_CHAR || c == NULL_CHAR) break;
    if (c == ASTERIX || c == BACK_TICK || c == PIPE) break;

    text += advance();
  }

  if (!text.empty())
    tokens.push_back(makeToken(TokenType::TEXT, line, col, std::move(text)));
}

void Lexer::readTableDelimiter(TokenListRef tokens) {
  int line = m_line, col = m_column;
  std::string delimiter;

  while (!isAtEnd() && peek() != PIPE && peek() != NEW_LINE_CHAR)
    delimiter += advance();

  size_t start = delimiter.find_first_not_of(" \t");
  size_t end = delimiter.find_last_not_of(" \t");

  if (start != std::string::npos)
    delimiter = delimiter.substr(start, end - start + 1);

  tokens.push_back(
      makeToken(TokenType::TABLE_DELIMITER, line, col, std::move(delimiter))
  );
}

void Lexer::readTableCell(TokenListRef tokens) {
  int line = m_line, col = m_column;
  std::string cell;

  while (!isAtEnd() && peek() != PIPE && peek() != NEW_LINE_CHAR)
    cell += advance();

  size_t end = cell.find_last_not_of(" \t");
  if (end != std::string::npos) cell = cell.substr(0, end + 1);

  if (!cell.empty())
    tokens.push_back(makeToken(TokenType::TEXT, line, col, std::move(cell)));
}

// -- Public Methods -- //

TokenList Lexer::tokenize() {
  TokenList tokens;
  while (!isAtEnd()) {
    handleState(tokens);
  }
  tokens.push_back(makeToken(TokenType::END_OF_FILE));
  return tokens;
}
} // namespace QuickNotes::Markdown
