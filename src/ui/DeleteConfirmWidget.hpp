#pragma once

#include "ncurses.h"
#include "ui/DialogBox.hpp"
#include <string>
#include <string_view>
namespace QuickNotes::UI {

class DeleteConfirmWidget : public DialogBox {

  public:
    explicit DeleteConfirmWidget(WINDOW *parent);

    void draw() override;

    void setNoteTitle(std::string_view title);

  private:
    static constexpr std::string_view LABEL = "Delete Note";
    static constexpr std::string_view PROMPT =
        "Are you sure you want to delete this note?";
    static constexpr std::string_view OPTIONS = "[y] Yes    [n] No";
    static constexpr int DIALOG_HEIGHT = 7;
    static constexpr int DIALOG_WIDTH = 44;
    static constexpr int MARGIN = 2;
    std::string m_noteTitle;
};

} // namespace QuickNotes::UI
