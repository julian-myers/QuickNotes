#pragma once

#include <filesystem>
#include <sqlite3.h>

namespace QuickNotes::DB {

class Database {
  public:
    /// @brief Constructs a Database.
    Database();
    /// @brief Deconstructs a Database.
    ~Database();

    /// @brief return a pointer to the database.
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

    /// @brief Set location of database.
    static std::filesystem::path dbPath();

    /// @brief Create the notes database if it doesn't already exist.
    /// @param sql Schema for the db.
    void execute(const std::string &sql);
};

} // namespace QuickNotes::DB
