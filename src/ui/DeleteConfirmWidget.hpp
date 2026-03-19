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

    /// @brief Display an inline error message if the deletion failed.
    ///
    /// When set, OPTIONS changes to "[y] Retry  [n] Cancel". Pass an empty
    /// string to clear the error and return to the normal prompt.
    /// @param message The error returned by the repository.
    void setError(std::string_view message);

  private:
    static constexpr std::string_view LABEL = "Delete Note";
    static constexpr std::string_view OPTIONS_NORMAL = "[y] Yes    [n] No";
    static constexpr std::string_view OPTIONS_RETRY = "[y] Retry  [n] Cancel";
    static constexpr int DIALOG_HEIGHT = 7;
    static constexpr int DIALOG_WIDTH = 44;
    static constexpr int MARGIN = 2;
    std::string m_noteTitle;
    std::string PROMPT = "Delete";
    std::string m_errorMessage;
};

} // namespace QuickNotes::UI
