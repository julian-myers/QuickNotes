#pragma once

#include "ncurses.h"
#include "ui/DialogBox.hpp"
#include <string>
#include <string_view>
namespace QuickNotes::UI {

/// @brief Modal confirmation dialog for note deletion.
///
/// Displays the name of the note to be deleted along with yes/no options.
/// Purely presentational — the owning DeleteNoteState handles the actual
/// key input and deletion logic.
///
/// @see DialogBox
/// @see DeleteNoteState
class DeleteConfirmWidget : public DialogBox {

  public:
    /// @brief Constructs a DeleteConfirmWidget centered in @p parent.
    /// @param parent Pointer to the parent ncurses WINDOW.
    explicit DeleteConfirmWidget(WINDOW *parent);

    /// @brief Renders the confirmation dialog with the current note title.
    void draw() override;

    /// @brief Sets the title of the note shown in the confirmation prompt.
    /// @param title The title of the note about to be deleted.
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
