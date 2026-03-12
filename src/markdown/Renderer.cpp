#include "Renderer.hpp"
#include "AST.hpp"
#include "Colors.hpp"
#include "config/Config.hpp"
#include <algorithm>
#include <array>
#include <ncurses.h>
#include <sstream>
#include <string>

using namespace QuickNotes::Markdown::Colors;

namespace {
std::string cellText(const QuickNotes::Markdown::TableCellNode &cell) {
  std::string text;
  for (const auto &child : cell.children) {
    if (auto *t =
            dynamic_cast<const QuickNotes::Markdown::TextNode *>(child.get())) {
      text += t->text;
    }
  }
  return text;
}
} // namespace

namespace QuickNotes::Markdown {

// --- Public Methods ---

// Constructor.
Renderer::Renderer(WINDOW *window, const Config::Config &config)
    : m_window(window), m_config(config) {
  start_color();
  if (can_change_color()) {
    Colors::initColorPairs(m_config);
    wbkgd(m_window, COLOR_PAIR(PAIR_TEXT));
  }
}

void Renderer::visit(const DocumentNode &node) {
  for (const auto &child : node.children) {
    child->accept(*this);
  }
}

void Renderer::visit(const HeadingNode &node) {
  static constexpr std::array<const char *, 6> HEADING_SYMBOLS = {
      "①", "②", "③", "④", "⑤", "⑥",
  };
  std::string prefix(node.level - 1, ' ');
  prefix += HEADING_SYMBOLS[node.level - 1];
  int pair;
  switch (node.level) {
    case 1:
      pair = PAIR_HEADING_1;
      break;
    case 2:
      pair = PAIR_HEADING_2;
      break;
    case 3:
      pair = PAIR_HEADING_3;
      break;
    case 4:
      pair = PAIR_HEADING_4;
      break;
    case 5:
      pair = PAIR_HEADING_5;
      break;
    case 6:
      pair = PAIR_HEADING_6;
      break;
    default:
      pair = PAIR_HEADING_6;
      break;
  }
  wattron(m_window, COLOR_PAIR(pair) | A_BOLD);
  wmove(m_window, m_currentRow, m_currentColumn);
  wprintw(m_window, "%s", prefix.c_str());
  m_currentColumn += (node.level - 1) + 2;
  for (const auto &child : node.children) {
    child->accept(*this);
  }
  wattroff(m_window, COLOR_PAIR(pair) | A_BOLD);
  m_currentRow++;
  m_currentColumn = COLUMN_START;
  m_currentRow++;
}

void Renderer::visit(const ParagraphNode &node) {
  wmove(m_window, m_currentRow, m_currentColumn);
  for (const auto &child : node.children) {
    child->accept(*this);
  }
  m_currentRow += EMPTY_LINE;
  m_currentColumn = COLUMN_START;
}

void Renderer::visit(const CodeBlockNode &node) {
  std::string code;
  int width = getmaxx(m_window);
  for (const auto &child : node.children) {
    if (auto *text = dynamic_cast<const TextNode *>(child.get())) {
      code = text->text;
    }
  }
  std::vector<std::string> lines;
  std::istringstream stream(code);
  std::string line;
  while (std::getline(stream, line)) {
    lines.push_back(line);
  }
  wattron(m_window, COLOR_PAIR(PAIR_CODE_BLOCK));
  for (const auto &codeLine : lines) {
    mvwprintw(m_window, m_currentRow, COLUMN_START, "%-*s", width,
              codeLine.c_str());
    m_currentRow++;
  }
  wattroff(m_window, COLOR_PAIR(PAIR_CODE_BLOCK));
  m_currentRow += EMPTY_LINE;
  m_currentColumn = COLUMN_START;
}

void Renderer::visit(const BlockQuoteNode &node) {
  m_indentLevel++;
  m_currentColumn = m_indentLevel * 2;
  for (const auto &child : node.children) {
    wattron(m_window, COLOR_PAIR(PAIR_BLOCK_QUOTE_LEFT_BORDER));
    mvwaddch(m_window, m_currentRow, COLUMN_START, ACS_VLINE);
    wattroff(m_window, COLOR_PAIR(PAIR_BLOCK_QUOTE_LEFT_BORDER));
    child->accept(*this);
  }
  m_indentLevel--;
  m_currentColumn = m_indentLevel * 2;
  m_currentRow++;
}

void Renderer::visit(const ListNode &node) {
  m_listKindStack.push(node.kind);
  m_listDepth++;
  m_listIndex = 1;
  for (const auto &item : node.items) {
    item->accept(*this);
  }
  m_listDepth--;
  m_listKindStack.pop();
  m_currentRow++;
}

void Renderer::visit(const ListItemNode &node) {
  static constexpr std::array<const char *, 4> BULLETS = {"●", "○", "◆", "◇"};
  int indent = m_listDepth * 2;
  ListKind kind = m_listKindStack.top();
  wattron(m_window, COLOR_PAIR(PAIR_LIST_BULLETS));
  if (kind == ListKind::UNORDERED) {
    int depth = std::clamp(m_listDepth - 1, 0, (int)BULLETS.size() - 1);
    mvwprintw(m_window, m_currentRow, indent, "%s", BULLETS[depth]);
  } else {
    mvwprintw(m_window, m_currentRow, indent, "%d.", m_listIndex++);
  }
  wattroff(m_window, COLOR_PAIR(PAIR_LIST_BULLETS));
  m_currentColumn = indent + 3;
  for (const auto &child : node.children) {
    child->accept(*this);
  }
  m_currentRow++;
  m_currentColumn = COLUMN_START;
}

void Renderer::visit(const TableNode &node) {
  m_columnWidths.clear();
  m_columnAlignments = node.columnAlignments;

  for (const auto &rowPtr : node.rows) {
    const auto *row = dynamic_cast<const TableRowNode *>(rowPtr.get());
    if (!row)
      continue;
    for (size_t i = 0; i < row->cells.size(); i++) {
      const auto *cell =
          dynamic_cast<const TableCellNode *>(row->cells[i].get());
      if (!cell)
        continue;
      int width = static_cast<int>(cellText(*cell).size());
      if (i >= m_columnWidths.size()) {
        m_columnWidths.push_back(width);
      } else {
        m_columnWidths[i] = std::max(m_columnWidths[i], width);
      }
    }
  }
  auto drawHBorder = [&](chtype left, chtype mid, chtype right) {
    wattron(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
    mvwaddch(m_window, m_currentRow, 0, left);
    int col = 1;
    for (size_t i = 0; i < m_columnWidths.size(); i++) {
      mvwhline(m_window, m_currentRow, col, ACS_HLINE, m_columnWidths[i] + 2);
      col += m_columnWidths[i] + 2;
      mvwaddch(m_window, m_currentRow, col,
               i < m_columnWidths.size() - 1 ? mid : right);
      col++;
    }
    wattroff(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
    m_currentRow++;
  };

  drawHBorder(ACS_ULCORNER, ACS_TTEE, ACS_URCORNER);

  for (const auto &row : node.rows) {
    row->accept(*this);
  }

  drawHBorder(ACS_LLCORNER, ACS_BTEE, ACS_LRCORNER);

  m_currentRow++;
  m_currentColumn = 0;
}

void Renderer::visit(const TableRowNode &node) {
  m_inTableHeader = node.isHeader;
  m_currentCell = 0;
  wattron(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
  mvwaddch(m_window, m_currentRow, 0, ACS_VLINE);
  wattroff(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
  for (const auto &cell : node.cells) {
    cell->accept(*this);
  }
  m_currentRow++;
  if (node.isHeader) {
    wattron(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
    mvwaddch(m_window, m_currentRow, 0, ACS_LTEE);
    int col = 1;
    for (size_t i = 0; i < m_columnWidths.size(); i++) {
      mvwhline(m_window, m_currentRow, col, ACS_HLINE, m_columnWidths[i] + 2);
      col += m_columnWidths[i] + 2;
      mvwaddch(m_window, m_currentRow, col,
               i < m_columnWidths.size() - 1 ? ACS_PLUS : ACS_RTEE);
      col++;
    }
    wattroff(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
    m_currentRow++;
  }
  m_currentColumn = COLUMN_START;
}

void Renderer::visit(const TableCellNode &node) {
  int colWidth = m_currentCell < static_cast<int>(m_columnWidths.size())
                     ? m_columnWidths[m_currentCell]
                     : 0;

  ColumnAlignment alignment =
      m_currentCell < static_cast<int>(m_columnAlignments.size())
          ? m_columnAlignments[m_currentCell]
          : ColumnAlignment::NONE;

  std::string text = cellText(node);
  int padding = colWidth - static_cast<int>(text.size());
  int leftPad = 1, rightPad = 1;
  switch (alignment) {
    case ColumnAlignment::RIGHT:
      leftPad += padding;
      break;
    case ColumnAlignment::CENTER:
      leftPad += padding / 2;
      rightPad += padding - padding / 2;
      break;
    case ColumnAlignment::LEFT:
    case ColumnAlignment::NONE:
      rightPad += padding;
      break;
  }
  int colStart = 1;
  for (int i = 0; i < m_currentCell; i++) {
    colStart += m_columnWidths[i] + 3; // +2 padding +1 border
  }
  int pair = m_inTableHeader ? PAIR_TABLE_HEADER : PAIR_TABLE_BODY;
  wattron(m_window, COLOR_PAIR(pair));
  mvwprintw(m_window, m_currentRow, colStart, "%*s%s%*s", leftPad, "",
            text.c_str(), rightPad, "");
  wattroff(m_window, COLOR_PAIR(pair));
  wattron(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
  mvwaddch(m_window, m_currentRow,
           colStart + leftPad + static_cast<int>(text.size()) + rightPad,
           ACS_VLINE);
  wattroff(m_window, COLOR_PAIR(PAIR_TABLE_BORDERS));
  m_currentCell++;
}

void Renderer::visit(const HorizontalRuleNode &node) {
  int width = getmaxx(m_window);
  wattron(m_window, COLOR_PAIR(PAIR_HORIZONTAL_RULE));
  mvwhline(m_window, m_currentRow, COLUMN_START, ACS_HLINE, width);
  wattroff(m_window, COLOR_PAIR(PAIR_HORIZONTAL_RULE));
  m_currentRow += EMPTY_LINE;
  m_currentColumn = COLUMN_START;
}

void Renderer::visit(const TextNode &node) {
  wmove(m_window, m_currentRow, m_currentColumn);
  wprintw(m_window, "%s", node.text.c_str());
  m_currentColumn += static_cast<int>(node.text.size());
}

void Renderer::visit(const BoldNode &node) {
  wattron(m_window, COLOR_PAIR(PAIR_BOLD) | A_BOLD);
  for (const auto &child : node.children) {
    child->accept(*this);
  }
  wattroff(m_window, COLOR_PAIR(PAIR_BOLD) | A_BOLD);
}

void Renderer::visit(const ItalicNode &node) {
  wattron(m_window, COLOR_PAIR(PAIR_ITALIC) | A_ITALIC);
  for (const auto &child : node.children) {
    child->accept(*this);
  }
  wattroff(m_window, COLOR_PAIR(PAIR_ITALIC) | A_ITALIC);
}

void Renderer::visit(const ItalicBoldNode &node) {
  wattron(m_window, COLOR_PAIR(PAIR_BOLD_ITALIC) | A_ITALIC);
  wattron(m_window, A_BOLD);
  for (const auto &child : node.children) {
    child->accept(*this);
  }
  wattroff(m_window, A_BOLD);
  wattroff(m_window, COLOR_PAIR(PAIR_BOLD_ITALIC) | A_ITALIC);
}

void Renderer::visit(const InLineCodeNode &node) {
  wattron(m_window, COLOR_PAIR(PAIR_INLINE_CODE));
  wmove(m_window, m_currentRow, m_currentColumn);
  wprintw(m_window, "%s", node.code.c_str());
  wattroff(m_window, COLOR_PAIR(PAIR_INLINE_CODE));
  m_currentColumn += static_cast<int>(node.code.size());
}

// --- Private Methods ---

} // namespace QuickNotes::Markdown
