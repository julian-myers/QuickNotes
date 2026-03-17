#pragma once

#include <filesystem>
#include <sqlite3.h>

namespace QuickNotes::DB {

/// @brief RAII wrapper around a SQLite database connection.
///
/// Opens (or creates) the SQLite database at dbPath() on construction and
/// applies the notes, tags, and note_tags schemas via execute(). The raw
/// connection pointer is exposed through connection() for use by
/// NotesRepository, which borrows it without taking ownership.
///
/// Only one Database instance should exist for the lifetime of the
/// application. NotesRepository must not outlive the Database that
/// provided its connection.
///
/// @see NotesRepository
class Database {
  public:
    /// @brief Opens the SQLite database and initializes the schema.
    Database();

    /// @brief Closes the SQLite database connection.
    ~Database();

    /// @brief Returns the raw SQLite connection pointer.
    ///
    /// The pointer is owned by this Database instance. Callers must not
    /// call sqlite3_close() on it.
    sqlite3 *connection();

  private:
    static constexpr const char *NOTES_SCHEMA = R"(
				 CREATE TABLE IF NOT EXISTS notes (
						id INTEGER PRIMARY KEY AUTOINCREMENT,
				 		title TEXT NOT NULL UNIQUE DEFAULT '',
				 		content TEXT NOT NULL DEFAULT '',
				 		created_at TEXT NOT NULL DEFAULT (datetime('now')),
				 		updated_at TEXT NOT NULL DEFAULT (datetime('now')),
				 		pinned INTEGER NOT NULL DEFAULT 0
				 );
		)";

    static constexpr const char *TAGS_SCHEMA = R"(
				 CREATE TABLE IF NOT EXISTS tags (
						id INTEGER PRIMARY KEY AUTOINCREMENT,
						name TEXT NOT NULL UNIQUE
				 );
		)";

    static constexpr const char *NOTE_TAGS_SCHEMA = R"(
				 CREATE TABLE IF NOT EXISTS note_tags (
						note_id INTEGER REFERENCES notes(id) ON DELETE CASCADE,
						tag_id INTEGER REFERENCES tags(id) ON DELETE CASCADE,
						PRIMARY KEY (note_id, tag_id)
				 );
		)";

    sqlite3 *m_db;

    /// @brief Returns the platform-appropriate path for the database file.
    ///
    /// Typically resolves to ~/.local/share/quicknotes/notes.db on Linux.
    static std::filesystem::path dbPath();

    /// @brief Execute a SQL statement against the open database connection.
    ///
    /// Used during construction to apply schema migrations. Throws
    /// std::runtime_error if SQLite reports an error.
    ///
    /// @param sql The SQL statement to execute.
    void execute(const std::string &sql);
};

} // namespace QuickNotes::DB
