#pragma once

#include <memory>
#include <string>
#include <vector>

namespace QuickNotes::Markdown {

struct DocumentNode;
struct HeadingNode;
struct ParagraphNode;
struct CodeBlockNode;
struct BlockQuoteNode;
struct ListNode;
struct ListItemNode;
struct TableNode;
struct TableRowNode;
struct TableCellNode;
struct HorizontalRuleNode;
struct TextNode;
struct BoldNode;
struct ItalicNode;
struct InLineCodeNode;
struct ItalicBoldNode;

/// @brief Wrapper struct to contain the abstract syntax tree.
struct AST {
    std::unique_ptr<DocumentNode> root;
};

/// @brief Interface for visiting all markdown AST node types.
///
/// Implement this interface to define behaviour for each node type.
/// The Visitor pattern allows new operations (e.g. rendering or exporting)
/// to be added without modifying the AST node types themselves.
///
/// @see DocumentNode
/// @see Node
class IVisitor {
  public:
    virtual ~IVisitor() = default;

    /// @brief Visit a node in the abstract syntax tree.
    ///
    /// Each overload is dispatched to by the corresponding node's accept()
    /// method. Implement all overrides to define behaviour for a complete
    /// traversal of the abstract syntax tree.
    ///
    ///	Accepts implementations of the Abstract Node struct as parameters.
    ///
    /// @see Node
    virtual void visit(const DocumentNode &) = 0;
    virtual void visit(const HeadingNode &) = 0;
    virtual void visit(const ParagraphNode &) = 0;
    virtual void visit(const CodeBlockNode &) = 0;
    virtual void visit(const BlockQuoteNode &) = 0;
    virtual void visit(const ListNode &) = 0;
    virtual void visit(const ListItemNode &) = 0;
    virtual void visit(const TableNode &) = 0;
    virtual void visit(const TableRowNode &) = 0;
    virtual void visit(const TableCellNode &) = 0;
    virtual void visit(const HorizontalRuleNode &) = 0;

    // Inline nodes
    virtual void visit(const TextNode &) = 0;
    virtual void visit(const BoldNode &) = 0;
    virtual void visit(const ItalicNode &) = 0;
    virtual void visit(const ItalicBoldNode &) = 0;
    virtual void visit(const InLineCodeNode &) = 0;
};

/// @brief Base class from which each AST node type inherits.
struct Node {
    /// @brief Deconstructor
    virtual ~Node() = default;

    /// @brief Accepts a visitor and dispatches to the appropriate visit()
    /// overload.
    /// @param visitor The visitor to dispatch to.
    virtual void accept(IVisitor &visitor) const = 0;
};

using NodePtr = std::unique_ptr<Node>;
using NodeList = std::vector<NodePtr>;

struct DocumentNode : Node {
    NodeList children;

    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); };
};

struct HeadingNode : Node {
    int level;
    NodeList children;

    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); };
};

enum class CodeLanguage {
  PYTHON,
  CPP,
  C,
  JAVA,
  JAVASCRIPT,
  LUA,
  RUST,
  RUBY,
  C_SHARP,
  HTML,
  BASH,
  CSS,
  NONE,
};

struct ParagraphNode : Node {
    NodeList children;
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); };
};

struct CodeBlockNode : Node {
    CodeLanguage language = CodeLanguage::NONE;
    NodeList children;
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); };
};

struct BlockQuoteNode : Node {
    NodeList children;
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); };
};

enum class ListKind { ORDERED, UNORDERED };

struct ListNode : Node {
    ListKind kind;
    NodeList items;
    explicit ListNode(ListKind kind) : kind(kind) {}
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); };
};

struct ListItemNode : Node {
    NodeList children;
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

enum class ColumnAlignment { NONE, LEFT, CENTER, RIGHT };

struct TableNode : Node {
    std::vector<ColumnAlignment> columnAlignments;
    NodeList rows;

    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct TableRowNode : Node {
    bool isHeader = false;
    NodeList cells;

    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct TableCellNode : Node {
    NodeList children;

    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct HorizontalRuleNode : Node {
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct TextNode : Node {
    std::string value;

    explicit TextNode(std::string text) : value(std::move(text)) {}
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct BoldNode : Node {
    NodeList children;

    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct ItalicNode : Node {
    NodeList children;

    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct ItalicBoldNode : Node {
    NodeList children;
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

struct InLineCodeNode : Node {
    std::string value;

    explicit InLineCodeNode(std::string code) : value(std::move(code)) {}
    /// @inheritdoc
    void accept(IVisitor &visitor) const override { visitor.visit(*this); }
};

template <typename T, typename... Args> NodePtr make_node(Args &&...args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

} // namespace QuickNotes::Markdown
