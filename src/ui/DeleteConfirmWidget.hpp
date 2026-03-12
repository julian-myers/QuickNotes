#pragma once

#include "ncurses.h"
#include "ui/Widget.hpp"
#include <string>
namespace QuickNotes::UI {

class DeleteConfirmWidget : public Widget {

  public:
    explicit DeleteConfirmWidget(WINDOW *window);

    ~DeleteConfirmWidget();

    void draw() override;

  private:
    static constexpr const char *LABEL = "Delete Note";
    static constexpr const char *PROMPT =
        "Are you sure you want to delete this note?";
    static constexpr const char *OPTIONS = "[y] Yes    [n] No";
    static constexpr int DIALOG_HEIGHT = 7;
    static constexpr int DIALOG_WIDTH = 44;
    static constexpr int MARGIN = 2;

    WINDOW *m_dialog;
    void draw(const std::string &title);
};

} // namespace QuickNotes::UI
