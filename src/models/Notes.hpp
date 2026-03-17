#pragma once

#include <string>

namespace QuickNotes::Model {

/// @brief Plain data structure representing a single note.
///
/// Mirrors the `notes` table row in the SQLite database. All fields are
/// populated by NotesRepository when notes are fetched from the database.
/// The @p id field is assigned by SQLite on creation and should be treated
/// as read-only by application code outside the repository layer.
struct Note {
    int id;           ///< Primary key from the notes table.
    std::string title;     ///< Unique title of the note.
    std::string content;   ///< Markdown body of the note.
    std::string createdAt; ///< ISO-8601 timestamp of creation (UTC).
    std::string updatedAt; ///< ISO-8601 timestamp of last edit (UTC).
};

} // namespace QuickNotes::Model
