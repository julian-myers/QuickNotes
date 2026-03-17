#pragma once

#include <string>
namespace QuickNotes::Config::Editor {

/// @brief Open the user's preferred terminal editor with pre-populated content.
///
/// Writes @p initialContent to a temporary file, suspends ncurses, and launches
/// the editor specified by the $EDITOR environment variable (falling back to
/// "nano" if unset). When the editor exits, ncurses is restored and the edited
/// content is read back from the temporary file and returned.
///
/// @param initialContent The text to pre-populate in the editor.
/// @return The content of the file after the editor closes.
std::string openEditor(const std::string &initialContent);

}
