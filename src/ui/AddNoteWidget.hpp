#pragma once

#include "Widget.hpp"
#include "ncurses.h"
#include <string>

namespace QuickNotes::UI {

class AddNoteWidget : public Widget {

  public:
    explicit AddNoteWidget(WINDOW *window);
    ~AddNoteWidget();
    void draw() override;
    std::string getInput();

  private:
    static constexpr const char *LABEL = "New Note";
    static constexpr const char *PROMPT = "Title: ";
    static constexpr int DIALOG_HEIGHT = 5;
    static constexpr int DIALOG_WIDTH = 44;
    static constexpr int MARGIN = 2;
    WINDOW *m_dialog;
};

} // namespace QuickNotes::UI
