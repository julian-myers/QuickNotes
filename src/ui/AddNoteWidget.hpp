#pragma once

#include "Widget.hpp"
#include "ncurses.h"
#include "ui/DialogBox.hpp"
#include <string>
#include <string_view>

namespace QuickNotes::UI {

class AddNoteWidget : public DialogBox {

  public:
    explicit AddNoteWidget(WINDOW *parent);
    void draw() override;
    void setInputBuffer(std::string_view buffer);

  private:
    static constexpr std::string_view LABEL = "New Note";
    static constexpr std::string_view PROMPT = "Title: ";
    static constexpr int DIALOG_HEIGHT = 5;
    static constexpr int DIALOG_WIDTH = 44;
    static constexpr int MARGIN = 2;
    std::string m_inputBuffer;
};

} // namespace QuickNotes::UI
