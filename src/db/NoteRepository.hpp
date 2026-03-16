#pragma once

#include "db/Database.hpp"
#include "db/INoteRepository.hpp"
#include "models/Notes.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>

namespace QuickNotes::DB {

/// @brief Provides CRUD operations for the database's note table.
///
/// Wraps the SQLite C API to expose a typed interface over the notes table.
/// All queries are prepared via sqlite3_prepare_v2 and finalized automatically
/// by the Statement RAII wrapper struct.
///
/// Methods that can fail due to expected conditions (duplicate title, note not
/// found) return std::expected<Model::Note, std::string>. Methods that return
/// multiple rows return std::vector<Model::Note>. Preparation failures throw
/// std::runtime_error -- callers should catch at the state layer.
///
/// The repository borrows its database connection from Database via a raw
/// sqlite3*. It does not own the connection and must not outlive the Database
/// instance that provided it.
///
/// @see Database
/// @see Model::Note.
class NotesRepository : public INotesRepository {

  public:
    /// @brief Constructs a NotesRepository.
    ///
    /// @param db reference to instance of Databse that owns the connection.
    NotesRepository(Database &db);

    /// @brief Deconstructs a NotesRepository.
    ~NotesRepository() = default;

    /// @brief Creates a new note and adds it to the database with title and
    /// empty content.
    ///
    /// @param title The title of the note. Must be unique.
    /// @return The created note upon successful addition to the database.
    /// @return An error string on failure. For example, if the title already
    /// exists or the database connection failed.
    INotesRepository::NoteResult create(const std::string &title) override;

    /// @brief Removes a note from the database.
    ///
    /// @param note copy of a Note.
    /// @return The deleted note on success.
    /// @return An error string on failure. For example: if no note with the id
    ///  exits.
    INotesRepository::NoteResult remove(const Model::Note note) override;

    /// @brief Find a note by its id.
    ///
    /// @param id note.id which is the primary key for the notes table.
    /// @return The note which has the passed ID.
    /// @return An error string on failure if, for example, the database
    /// connection failed orif a note with the passed id doesn't exist.
    INotesRepository::NoteResult findById(int id) override;

    /// @brief Query the database for all notes.
    ///
    /// @return a vector containing all the notes in the database. Empty if
    /// notes notes exist yet.
    std::vector<Model::Note> findAll() override;

    /// @brief Find the most recently edited notes in the db.
    ///
    /// @param amount Number of most recent to pull.
    /// @return A vector of size amount containing the most recently edited
    /// notes ordered by updated_at descending.
    std::vector<Model::Note> findMostRecent(int amount) override;

    /// @brief Edit / update a note.
    ///
    /// Updates the notes content and updated_at.
    ///
    /// @param note A reference to the note to be updated.
    /// @return The updated note on success.
    /// @return An error string on failure.
    INotesRepository::NoteResult update(const Model::Note &note) override;

    /// @brief Find all notes whose title partially/fully matches the query
    /// string.
    ///
    /// @param query the title of the note the user is searching for. Can be
    /// partial.
    /// @return a vector<Model::Note> of potential matches.
    std::vector<Model::Note> findByTitle(const std::string &query) override;

  private:
    /// @brief RAII wrapper for sqlite3_stmt.
    ///
    /// Deconstructor calls sqlite3_finalize.
    struct Statement {
        sqlite3_stmt *statement = nullptr;
        Statement() = default;
        Statement(const Statement &) = delete;
        Statement &operator=(const Statement &) = delete;
        Statement(Statement &&) = default;
        Statement &operator=(Statement &&) = default;
        ~Statement() { sqlite3_finalize(statement); }
    };

    /// @brief Interprets sql row into a note.
    ///
    /// @param statement A pointer to an sqlite3_stmt.
    /// @return A new instance of Model::Note.
    Model::Note toNote(sqlite3_stmt *statement);

    /// @brief Collects the notes from multiple rows in the table.
    ///
    /// @param statement instance of Statement.
    /// @return a vector containing the Notes created from the rows.
    std::vector<Model::Note> collect(Statement &statement);

    /// @brief Compile a std::string query into an sqlite3_stmt.
    ///
    /// @param query reference to the query std::string.
    /// @return Instance of Statement which provides an RAII wrapper for the
    /// sqlite3_stmt.
    Statement prepare(const std::string &query);

    sqlite3 *m_db;
};
} // namespace QuickNotes::DB
