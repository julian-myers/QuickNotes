#include "db/Database.hpp"
#include "utils/Log.hpp"
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include <stdexcept>

namespace QuickNotes::DB {

Database::Database() : m_db(nullptr) {
  auto path = dbPath();
  std::filesystem::create_directories(path.parent_path());
  QN_LOG_INFO("Opening database at {}", path.string());
  if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK) {
    QN_LOG_ERROR("Failed to open database: {}", sqlite3_errmsg(m_db));
    throw std::runtime_error(sqlite3_errmsg(m_db));
  }
  QN_LOG_DEBUG("Database opened successfully");
  execute(NOTES_SCHEMA);
}

Database::~Database() { sqlite3_close(m_db); }

sqlite3 *Database::connection() { return m_db; }

std::filesystem::path Database::dbPath() {
  const char *xdg = std::getenv("XDG_DATA_HOME");
  if (xdg) {
    return std::filesystem::path(xdg) / "QuickNotes" / "notes.db";
  }
  const char *home = std::getenv("HOME");
  if (!home) {
    throw std::runtime_error("Neither XDG_DATA_HOME nor HOME is set");
  }
  return std::filesystem::path(home) / ".local/share" / "QuickNotes" / "notes.db";
}

void Database::execute(const std::string &sql) {
  char *errMsg = nullptr;
  int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK) {
    std::string error = errMsg;
    sqlite3_free(errMsg);
    QN_LOG_ERROR("Database::execute failed: {}", error);
    throw std::runtime_error("Database::execute failed: " + error);
  }
}

} // namespace QuickNotes::DB
