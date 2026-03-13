#pragma once

#include "models/Notes.hpp"
#include <expected>
#include <string>
#include <vector>

namespace QuickNotes::DB {

/// @brief Interface for CRUD operations over the notes table.
///
/// Implement this interface to define a notes persistence backend.
/// All single-result operations return NoteResult (std::expected<Model::Note,
/// std::string>). Failures due to expected conditions (duplicate title, note
/// not found) are returned as error strings. Callers should handle these at the
/// state layer.
///
/// @see Model::Note
/// @see NotesRepository
class INotesRepository {
  public:
    /// @brief Deconstructs a NotesRepository.
    virtual ~INotesRepository() = default;

    using NoteResult = std::expected<Model::Note, std::string>;

    /// @brief Creates a new note and adds it to the database with title and
    /// empty content.
    ///
    /// @param title The title of the note. Must be unique.
    /// @return The created note upon successful addition to the database.
    /// @return An error string on failure. For example, if the title already
    /// exists or the database connection failed.
    virtual NoteResult create(const std::string &title) = 0;

    /// @brief Removes a note from the database.
    ///
    /// @param note copy of a Note.
    /// @return The deleted note on success.
    /// @return An error string on failure. For example: if no note with the id
    ///  exits.
    virtual NoteResult remove(const Model::Note note) = 0;

    /// @brief Find a note by its id.
    ///
    /// @param id note.id which is the primary key for the notes table.
    /// @return The note which has the passed ID.
    /// @return An error string on failure if, for example, the database
    /// connection failed orif a note with the passed id doesn't exist.
    virtual NoteResult findById(int id) = 0;

    /// @brief Edit / update a note.
    ///
    /// Updates the notes content and updated_at.
    ///
    /// @param note A reference to the note to be updated.
    /// @return The updated note on success.
    /// @return An error string on failure.
    virtual NoteResult update(const Model::Note &note) = 0;

    /// @brief Query the database for all notes.
    ///
    /// @return a vector containing all the notes in the database. Empty if
    /// notes notes exist yet.
    virtual std::vector<Model::Note> findAll() = 0;

    /// @brief Find the most recently edited notes in the db.
    ///
    /// @param amount Number of most recent to pull.
    /// @return A vector of size amount containing the most recently edited
    /// notes ordered by updated_at descending.
    virtual std::vector<Model::Note> findMostRecent(int amount) = 0;
};

} // namespace QuickNotes::DB
