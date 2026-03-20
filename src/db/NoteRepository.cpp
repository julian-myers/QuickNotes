#include "db/NoteRepository.hpp"
#include "db/Database.hpp"
#include "utils/Log.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <string>

namespace QuickNotes::DB {

using Result = std::expected<Model::Note, std::string>;

// Constructor.
NotesRepository::NotesRepository(Database &db)
    : m_db(db.connection()) {}

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
  return findById(note.id);
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
  const std::string query =
      std::string(BASE_SELECT) + "WHERE n.id = ? GROUP BY n.id";
  auto s = prepare(query);
  sqlite3_bind_int(s.statement, 1, id);
  if (sqlite3_step(s.statement) == SQLITE_ROW) {
    return toNote(s.statement);
  }
  QN_LOG_WARN("Note not found id={}", id);
  return std::unexpected("Note with id " + std::to_string(id) + " not found");
}

std::vector<Model::Note> NotesRepository::findAll() {
  const std::string query = std::string(BASE_SELECT) + "GROUP BY n.id";
  auto s = prepare(query);
  return collect(s);
}

std::vector<Model::Note> NotesRepository::findMostRecent(int amount) {
  const std::string query = std::string(BASE_SELECT) +
                            "GROUP BY n.id ORDER BY n.updated_at DESC LIMIT ?";
  auto s = prepare(query);
  sqlite3_bind_int(s.statement, 1, amount);
  return collect(s);
}

std::vector<Model::Note>
NotesRepository::findByTitle(const std::string &query) {
  const std::string sqlQuery =
      std::string(BASE_SELECT) + "WHERE n.title LIKE ? GROUP BY n.id";
  const std::string pattern = "%" + query + "%";
  auto s = prepare(sqlQuery);
  sqlite3_bind_text(s.statement, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
  return collect(s);
}

// ------- Private Methods -------

Model::Note NotesRepository::toNote(sqlite3_stmt *statement) {
  return Model::Note{
      .id = sqlite3_column_int(statement, ID_COLUMN_NUM),
      .pinned = sqlite3_column_int(statement, PINNED_COLUMN_NUM) != 0,
      .title = reinterpret_cast<const char *>(
          sqlite3_column_text(statement, TITLE_COLUMN_NUM)
      ),
      .content = reinterpret_cast<const char *>(
          sqlite3_column_text(statement, CONTENT_COLUMN_NUM)
      ),
      .createdAt = reinterpret_cast<const char *>(
          sqlite3_column_text(statement, CREATED_COLUMN_NUM)
      ),
      .updatedAt = reinterpret_cast<const char *>(
          sqlite3_column_text(statement, UPDATE_COLUMN_NUM)
      ),
      .tags = splitTags(
          reinterpret_cast<const char *>(
              sqlite3_column_text(statement, TAGGED_COLUMN_NUM)
          )
      )
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

std::vector<std::string> NotesRepository::splitTags(const char *raw) {
  if (!raw) return {};
  std::vector<std::string> tags;
  std::string s(raw);
  std::string::size_type start = 0, end;
  while ((end = s.find(',', start)) != std::string::npos) {
    tags.push_back(s.substr(start, end - start));
    start = end + 1;
  }
  tags.push_back(s.substr(start));
  return tags;
}

void NotesRepository::setPinned(int noteId, bool pinned) {
  QN_LOG_DEBUG("setPinned id={} pinned={}", noteId, pinned);
  const std::string query = "UPDATE notes SET pinned = ? WHERE id = ?";
  auto s = prepare(query);
  sqlite3_bind_int(s.statement, 1, pinned ? 1 : 0);
  sqlite3_bind_int(s.statement, 2, noteId);
  if (sqlite3_step(s.statement) != SQLITE_DONE) {
    QN_LOG_WARN(
        "Failed to set pinned id={}: {}", noteId, sqlite3_errmsg(m_db)
    );
  }
}

void NotesRepository::addTag(int noteId, const std::string &name) {
  QN_LOG_DEBUG("addTag id={} name=\"{}\"", noteId, name);
  // Create tag if it doesn't exist yet.
  const std::string insertTag = "INSERT OR IGNORE INTO tags (name) VALUES (?)";
  auto s1 = prepare(insertTag);
  sqlite3_bind_text(s1.statement, 1, name.c_str(), -1, SQLITE_STATIC);
  sqlite3_step(s1.statement);

  // Link the tag to the note.
  const std::string link =
      "INSERT OR IGNORE INTO note_tags (note_id, tag_id) "
      "SELECT ?, id FROM tags WHERE name = ?";
  auto s2 = prepare(link);
  sqlite3_bind_int(s2.statement, 1, noteId);
  sqlite3_bind_text(s2.statement, 2, name.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(s2.statement) != SQLITE_DONE) {
    QN_LOG_WARN(
        "Failed to add tag \"{}\" to id={}: {}", name, noteId,
        sqlite3_errmsg(m_db)
    );
  }
}

void NotesRepository::removeTag(int noteId, const std::string &name) {
  QN_LOG_DEBUG("removeTag id={} name=\"{}\"", noteId, name);
  const std::string query =
      "DELETE FROM note_tags WHERE note_id = ? AND "
      "tag_id = (SELECT id FROM tags WHERE name = ?)";
  auto s = prepare(query);
  sqlite3_bind_int(s.statement, 1, noteId);
  sqlite3_bind_text(s.statement, 2, name.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(s.statement) != SQLITE_DONE) {
    QN_LOG_WARN(
        "Failed to remove tag \"{}\" from id={}: {}", name, noteId,
        sqlite3_errmsg(m_db)
    );
  }
}

std::vector<std::string> NotesRepository::getAllTags() {
  const std::string query = "SELECT name FROM tags ORDER BY name";
  auto s = prepare(query);
  std::vector<std::string> tags;
  while (sqlite3_step(s.statement) == SQLITE_ROW) {
    tags.emplace_back(
        reinterpret_cast<const char *>(sqlite3_column_text(s.statement, 0))
    );
  }
  return tags;
}

std::vector<Model::Note> NotesRepository::findByTag(const std::string &tag) {
  const std::string query =
      std::string(BASE_SELECT) +
      "WHERE n.id IN (SELECT nt2.note_id FROM note_tags nt2 "
      "JOIN tags t2 ON nt2.tag_id = t2.id WHERE t2.name = ?) "
      "GROUP BY n.id";
  auto s = prepare(query);
  sqlite3_bind_text(s.statement, 1, tag.c_str(), -1, SQLITE_STATIC);
  return collect(s);
}

} // namespace QuickNotes::DB
