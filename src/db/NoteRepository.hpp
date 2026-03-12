#pragma once

#include "db/Database.hpp"
#include "models/Notes.hpp"
#include <expected>
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
class NotesRepository {

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
    std::expected<Model::Note, std::string> create(const std::string &title);

    /// @brief Removes a note from the database.
    ///
    /// @param note copy of a Note.
    /// @return The deleted note on success.
    /// @return An error string on failure. For example: if no note with the id
    ///  exits.
    std::expected<Model::Note, std::string> remove(const Model::Note note);

    /// @brief Find a note by its id.
    ///
    /// @param id note.id which is the primary key for the notes table.
    /// @return The note which has the passed ID.
    /// @return An error string on failure if, for example, the database
    /// connection failed orif a note with the passed id doesn't exist.
    std::expected<Model::Note, std::string> findById(int id);

    /// @brief Query the database for all notes.
    ///
    /// @return a vector containing all the notes in the database. Empty if
    /// notes notes exist yet.
    std::vector<Model::Note> findAll();

    /// @brief Find the most recently edited notes in the db.
    ///
    /// @param amount Number of most recent to pull.
    /// @return A vector of size amount containing the most recently edited
    /// notes ordered by updated_at descending.
    std::vector<Model::Note> findMostRecent(int amount);

    /// @brief Edit / update a note.
    ///
    /// Updates the notes content and updated_at.
    ///
    /// @param note A reference to the note to be updated.
    /// @return The updated note on success.
    /// @return An error string on failure.
    std::expected<Model::Note, std::string> update(const Model::Note &note);

  private:
    /// @brief RAII wrapper for sqlite3_stmt.
    ///
    /// Deconstructor calls sqlite3_finalize.
    struct Statement {
        sqlite3_stmt *statement = nullptr;
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
