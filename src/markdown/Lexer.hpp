#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace QuickNotes::Markdown {

/// @brief Set of tokens in a markdown file.
enum class TokenType {
  HEADING_1,
  HEADING_2,
  HEADING_3,
  HEADING_4,
  HEADING_5,
  HEADING_6,
  HORIZONTAL_RULE,
  BLOCK_QUOTE,
  LIST_BULLET,
  LIST_ORDERED,
  TABLE_PIPE,
  TABLE_DELIMITER,
  CODE_FENCE,
  CODE_FENCE_LANG,
  BOLD,
  ITALIC,
  BOLD_ITALIC,
  INLINE_CODE,
  TEXT,
  NEWLINE,
  BLANK_LINE,
  END_OF_FILE,
};

/// @brief Struct to represent a token.
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

using TokenList = std::vector<Token>;
using TokenListRef = std::vector<Token> &;

/// @brief Tokenizes a Markdown string into a flat sequence of tokens.
///
/// The lexer operates as a state machine with three states:
/// - DEFAULT: standard line-by-line tokenization
/// - IN_CODE_BLOCK: captures raw code content until a closing fence
/// - IN_TABLE: tokenizes pipe-delimited table rows
///
/// Recognized token types (the lexer's alphabet):
/// - Block:  HEADING_1-6, HORIZONTAL_RULE, BLOCK_QUOTE, CODE_FENCE,
///           TABLE_PIPE, TABLE_DELIMITER, LIST_BULLET, LIST_ORDERED
/// - Inline: BOLD, ITALIC, BOLD_ITALIC, INLINE_CODE, TEXT
/// - Flow:   NEWLINE, BLANK_LINE, END_OF_FILE
///
/// A BLANK_LINE (two consecutive newlines) acts as a block separator.
/// The lexer does not perform any parsing — it produces a flat token
/// stream consumed by Parser.
///
/// @see Parser
/// @see TokenType
class Lexer {

  public:
    /// @brief Constructs a Lexer.
    /// @param source the Markdown to tokenize.
    explicit Lexer(std::string source);

    /// @brief Tokenizes the source markdown file/string.
    /// @return A vector of tokens to be passed into the parser.
    std::vector<Token> tokenize();

  private:
    /// @brief The three states of the lexer.
    enum class LexerState {
      DEFAULT,
      IN_CODE_BLOCK,
      IN_TABLE,
    };

    std::string m_source; // raw markdown
    size_t m_position;    // current position
    int m_line;           // current line number
    int m_column;         // current column number
    LexerState m_state;   // current state of the lexer.

    /// @brief Get a copy of the char at the current position of the lexer.
    /// @return a copy of the the char at the current position of the lexer if
    /// the lexer is not at the end, otherwise returns the null char '\0'
    /// specifically.
    /// @see isAtEnd()
    char peek() const;

    /// @brief Get a copy of the char one position ahead of the lexer's current
    /// position.
    /// @return a copy of the char at the lexer's current position if the lexer
    /// is not at the end. Otherwise the null char is returned.
    /// @see isAtEnd()
    char peekNext() const;

    /// @brief Get a copy of the char at the lexer's current position and
    /// increment the lexer's position forward by one.
    ///
    /// Additionally, this method tracks the column and line number of the
    /// source string for which the lexer is currently positioned.
    ///
    /// @return A copy of the char at the lexer's current position.
    char advance();

    /// @brief Checks if the lexer's position is at the end of the string.
    ///
    /// @return boolean
    bool isAtEnd() const;

    /// @brief Instantiate an instance of the Token class.
    /// @param type One of the members of the TokenType enum.
    /// @param value The string for which value field of the Token instance
    /// should hold.
    /// @return An instance of Token
    /// @see Token
    /// @see TokenType
    Token makeToken(TokenType type, std::string value = "");

    /// @brief Instantiate a Token with an explicit source position.
    /// @param type One of the members of the TokenType enum.
    /// @param line The line number where the token begins.
    /// @param col The column number where the token begins.
    /// @param value The string the value field should hold.
    /// @return An instance of Token.
    Token makeToken(TokenType type, int line, int col, std::string value = "");

    /// @brief Handle the current state of the lexer.
    /// @param tokens A reference to the vector of tokens.
    /// @throws std::runtime_error if value of m_state is not a valid lexer
    /// state.
    /// @see Lexer::LexerState
    void handleState(TokenListRef tokens);

    /// @brief Handle the default state of the lexer.
    ///
    /// @param tokens A reference to the vector of tokens.
    /// @see readNewLine()
    /// @see readHeading()
    /// @see readBlockQuote()
    /// @see readBackTick()
    /// @see readEmphasis()
    /// @see readPipe()
    /// @see readDash()
    /// @see readOrderedList()
    /// @see readText()
    void handleDefaultState(TokenListRef tokens);

    /// @brief Handle tokens while lexer is in code block state.
    /// @param tokens a reference to the vector of tokens.
    void handleCodeBlockState(TokenListRef tokens);

    /// @brief Handle tokens while lexer is in table state.
    /// @param tokens a reference to the vector of tokens.
    void handleTableState(TokenListRef tokens);

    /// @brief Handles lexing the a new line.
    /// @param tokens reference to the vector of tokens.
    void readNewLine(TokenListRef tokens);

    /// @brief Handles lexing headers.
    /// @param tokens a reference to the vector tokens.
    void readHeading(TokenListRef tokens);

    /// @brief Lexes block quote.
    /// @param tokens a reference to the list of tokens.
    void readBlockQuote(TokenListRef tokens);

    /// @brief Lex a back tick
    /// @param tokens a reference to the list of tokens.
    void readBackTick(TokenListRef tokens);

    /// @brief Lex an asterix. Determine if bold, italic, or bold-italic.
    /// @param tokens a reference to the list of tokens.
    void readEmphasis(TokenListRef tokens);

    /// @brief Lex a pipe character. Determine if regular text, or whatever.
    /// @param tokens a reference to the list of tokens.
    void readPipe(TokenListRef tokens);

    /// @brief Lex a dash character. Determine what to do with the dash.
    /// @param tokens a reference to the list of tokens.
    void readDash(TokenListRef tokens);

    /// @brief Lex an ordered list.
    /// @param tokens a reference to the list of tokens.
    void readOrderedList(TokenListRef tokens);

    /// @brief read and store plain text.
    /// @param tokens a reference to the list of tokens.
    void readText(TokenListRef tokens);

    /// @brief Lex the content of a code block/in line code.
    /// @param tokens a reference to the list of tokens.
    void readCodeContent(TokenListRef tokens);

    /// @brief Lex a table cell.
    /// @param tokens a reference to the list of tokens.
    void readTableCell(TokenListRef tokens);

    /// @brief Lex a table delimiter.
    /// @param tokens a reference to the list of tokens.
    void readTableDelimiter(TokenListRef tokens);

    /// @brief Lex the end of a table.
    /// @param tokens a reference to the list of tokens.
    void readTableEnd(TokenListRef tokens);
};
} // namespace QuickNotes::Markdown
