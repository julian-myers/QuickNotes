#pragma once

#include "Widget.hpp"
#include "ncurses.h"
#include "ui/DialogBox.hpp"
#include <string>
#include <string_view>

namespace QuickNotes::UI {

/// @brief Modal dialog widget for entering a new note's title.
///
/// Renders a centered dialog with a title prompt and the current input buffer.
/// Purely presentational — NewNoteState owns the input buffer and passes it
/// in via setInputBuffer() before each draw() call.
///
/// @see DialogBox
/// @see NewNoteState
class AddNoteWidget : public DialogBox {

  public:
    /// @brief Constructs an AddNoteWidget centered in @p parent.
    /// @param parent Pointer to the parent ncurses WINDOW.
    explicit AddNoteWidget(WINDOW *parent);

    /// @brief Renders the dialog with the current input buffer.
    void draw() override;

    /// @brief Updates the title input buffer displayed in the dialog.
    /// @param buffer The current user input for the new note title.
    void setInputBuffer(std::string_view buffer);

    /// @brief Display an inline error message below the prompt.
    ///
    /// Pass an empty string to clear the error.
    /// @param message The error to display (e.g. "Title already exists").
    void setError(std::string_view message);

  private:
    static constexpr std::string_view LABEL = "New Note";
    static constexpr std::string_view PROMPT = "Title: ";
    static constexpr int DIALOG_HEIGHT = 6;
    static constexpr int DIALOG_WIDTH = 44;
    static constexpr int MARGIN = 2;
    std::string m_inputBuffer;
    std::string m_errorMessage;
};

} // namespace QuickNotes::UI
