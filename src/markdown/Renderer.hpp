#pragma once

#include "AST.hpp"
#include "config/Config.hpp"
#include <ncurses.h>
#include <stack>

namespace QuickNotes::Markdown {

/// @brief Renders a Markdown AST to an ncurses window via the Visitor pattern.
///
/// Implements IVisitor to traverse the AST produced by Parser and write
/// styled output to a WINDOW using ncurses primitives. Each visit() overload
/// is responsible for applying the appropriate color pair, advancing the
/// cursor, and delegating to child nodes where applicable.
///
/// Rendering state tracked across visits:
/// - m_currentRow / m_currentColumn: current cursor position
/// - m_indentLevel: indentation depth for block quotes
/// - m_listDepth / m_listKindStack: nesting depth and kind (ordered/unordered)
///   for mixed nested lists
/// - m_columnWidths / m_columnAlignments: pre-calculated column layout
///   for tables, computed in visit(TableNode) before any cells are drawn
/// - m_currentCell / m_inTableHeader: per-row cell index and header flag
///   used by visit(TableCellNode) to select the correct color pair
///
/// Color pairs are initialized from a Config::Config instance in the
/// constructor via initColorPairs(). Custom colors require a terminal
/// that supports can_change_color(). Color pair IDs and individual color
/// IDs are defined in Colors.hpp.
///
/// ncurses is not thread-safe — all visit() calls must occur on the
/// main thread.
///
/// @see IVisitor
/// @see Parser
/// @see Colors.hpp
/// @see Config::ColorConfig
class Renderer : public IVisitor {

  public:
    /// @brief Constructs a Renderer.
    /// @param window Pointer to an instance of ncurses WINDOW.
    /// @param config A reference to an instance of Config::Config
    explicit Renderer(WINDOW *window, const Config::Config &config);

    /// @inheritdoc
    void visit(const DocumentNode &node) override;

    void visit(const HeadingNode &node) override;

    void visit(const ParagraphNode &node) override;

    void visit(const CodeBlockNode &node) override;

    void visit(const BlockQuoteNode &node) override;

    void visit(const ListNode &node) override;

    void visit(const ListItemNode &node) override;

    void visit(const TableNode &node) override;

    void visit(const TableRowNode &node) override;

    void visit(const TableCellNode &node) override;

    void visit(const HorizontalRuleNode &node) override;

    void visit(const TextNode &node) override;

    void visit(const BoldNode &node) override;

    void visit(const ItalicNode &node) override;

    void visit(const ItalicBoldNode &node) override;

    void visit(const InLineCodeNode &node) override;

  private:
    static constexpr int COLUMN_START = 0; // First column

    // Number of rows to produce an empty line.
    static constexpr int EMPTY_LINE = 2;

    WINDOW *m_window;
    Config::Config m_config;
    int m_currentRow = 0;
    int m_currentColumn = 0;
    int m_indentLevel = 0;
    int m_listDepth = 0;
    int m_listIndex = 1;
    std::stack<ListKind> m_listKindStack;
    std::vector<int> m_columnWidths;
    std::vector<ColumnAlignment> m_columnAlignments;
    int m_currentCell = 0;
    bool m_inTableHeader = false;
};

} // namespace QuickNotes::Markdown
