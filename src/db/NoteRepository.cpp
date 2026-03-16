#include "db/NoteRepository.hpp"
#include "db/Database.hpp"
#include "utils/Log.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <string>

namespace QuickNotes::DB {

using Result = std::expected<Model::Note, std::string>;

// Constructor.
NotesRepository::NotesRepository(Database &db) : m_db(db.connection()) {}

Result NotesRepository::create(const std::string &title) {
  QN_LOG_DEBUG("Creating note title=\"{}\"", title);
  Statement s;
  const std::string query =
      "INSERT INTO notes (title, content, created_at, "
      "updated_at) VALUES (?, '', datetime('now'), datetime('now'))";
  int rc = sqlite3_prepare_v2(m_db, query.c_str(), -1, &s.statement, nullptr);

  if (rc != SQLITE_OK) {
    return std::unexpected(sqlite3_errmsg(m_db));
  }
  sqlite3_bind_text(s.statement, 1, title.c_str(), -1, SQLITE_STATIC);
  rc = sqlite3_step(s.statement);
  if (rc != SQLITE_DONE) {
    QN_LOG_WARN(
        "Failed to create note title=\"{}\": {}", title, sqlite3_errmsg(m_db)
    );
    return std::unexpected(sqlite3_errmsg(m_db));
  }
  int id = static_cast<int>(sqlite3_last_insert_rowid(m_db));
  QN_LOG_INFO("Created note id={}, title=\"{}\"", id, title);
  return findById(id);
}

Result NotesRepository::update(const Model::Note &note) {
  QN_LOG_DEBUG("Updating note id={}", note.id);
  const std::string query = "UPDATE notes SET title = ?, content = ?, "
                            "updated_at = datetime('now') WHERE id = ?";

  auto s = prepare(query);
  sqlite3_bind_text(s.statement, 1, note.title.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(s.statement, 2, note.content.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(s.statement, 3, note.id);
  int rc = sqlite3_step(s.statement);
  if (rc != SQLITE_DONE) {
    QN_LOG_WARN(
        "Failed to update note id={}: {}", note.id, sqlite3_errmsg(m_db)
    );
    return std::unexpected(sqlite3_errmsg(m_db));
  }
  QN_LOG_DEBUG("Updated note id={}", note.id);
  return note;
}

Result NotesRepository::remove(const Model::Note note) {
  QN_LOG_DEBUG("Removing note id={}, title=\"{}\"", note.id, note.title);
  return findById(note.id).and_then([&](Model::Note note) -> Result {
    const std::string query = "DELETE FROM notes WHERE id = ?";
    auto s = prepare(query);
    sqlite3_bind_int(s.statement, 1, note.id);
    int rc = sqlite3_step(s.statement);
    if (rc != SQLITE_DONE) {
      QN_LOG_WARN(
          "Failed to remove note id={}: {}", note.id, sqlite3_errmsg(m_db)
      );
      return std::unexpected(sqlite3_errmsg(m_db));
    }
    QN_LOG_INFO("Removed note id={}, title=\"{}\"", note.id, note.title);
    return note;
  });
}

Result NotesRepository::findById(int id) {
  QN_LOG_DEBUG("findById id={}", id);
  const std::string query = "SELECT id, title, content, created_at, updated_at "
                            "FROM notes WHERE id = ?";
  auto s = prepare(query);
  sqlite3_bind_int(s.statement, 1, id);
  if (sqlite3_step(s.statement) == SQLITE_ROW) {
    return toNote(s.statement);
  }
  return std::unexpected("Note to id " + std::to_string(id) + " not found");
  QN_LOG_WARN("Note not found id={}", id);
}

std::vector<Model::Note> NotesRepository::findAll() {
  const std::string query =
      "SELECT id, title, content, created_at, updated_at FROM notes";
  auto s = prepare(query);
  return collect(s);
}

std::vector<Model::Note> NotesRepository::findMostRecent(int amount) {
  const std::string query = "SELECT id, title, content, created_at, updated_at "
                            "FROM notes ORDER BY update_at DESC LIMT ?";
  auto s = prepare(query);
  sqlite3_bind_int(s.statement, 1, amount);
  return collect(s);
}

std::vector<Model::Note>
NotesRepository::findByTitle(const std::string &query) {
  const std::string sqlQuery =
      "SELECT id, title, content, created_at, updated_at "
      "FROM notes "
      "WHERE title "
      "LIKE ?";
  const std::string pattern = "%" + query + "%";
  auto s = prepare(sqlQuery);
  sqlite3_bind_text(s.statement, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
  return collect(s);
}

// ------- Private Methods -------

Model::Note NotesRepository::toNote(sqlite3_stmt *statement) {
  return Model::Note{
      sqlite3_column_int(statement, 0),
      reinterpret_cast<const char *>(sqlite3_column_text(statement, 1)),
      reinterpret_cast<const char *>(sqlite3_column_text(statement, 2)),
      reinterpret_cast<const char *>(sqlite3_column_text(statement, 3)),
      reinterpret_cast<const char *>(sqlite3_column_text(statement, 4)),
  };
}

std::vector<Model::Note> NotesRepository::collect(Statement &statement) {
  std::vector<Model::Note> notes;
  while (sqlite3_step(statement.statement) == SQLITE_ROW) {
    notes.push_back(toNote(statement.statement));
  }
  return notes;
}

NotesRepository::Statement NotesRepository::prepare(const std::string &query) {
  Statement statement;
  int rc = sqlite3_prepare_v2(
      m_db, query.c_str(), -1, &statement.statement, nullptr
  );
  if (rc != SQLITE_OK) {
    QN_LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(m_db));
    throw std::runtime_error(sqlite3_errmsg(m_db));
  }

  return statement;
}

// std::vector<Model::Note> findMostRecent(int amount) {}

} // namespace QuickNotes::DB
