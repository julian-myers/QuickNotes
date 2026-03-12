#include "db/Database.hpp"
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include <stdexcept>

namespace QuickNotes::DB {

Database::Database() : m_db(nullptr) {
  auto path = dbPath();
  std::filesystem::create_directories(path.parent_path());
  if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(m_db));
  }
  execute(NOTES_SCHEMA);
}

Database::~Database() { sqlite3_close(m_db); }

sqlite3 *Database::connection() { return m_db; }

std::filesystem::path Database::dbPath() {
  const char *xdg = std::getenv("XDG_DATA_HOME");
  std::filesystem::path dataDir =
      xdg ? std::filesystem::path(xdg)
          : std::filesystem::path(std::getenv("HOME")) / ".local/share";
  return dataDir / "QuickNotes" / "notes.db";
}

void Database::execute(const std::string &sql) {
  char *errMsg = nullptr;
  int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK) {
    std::string error = errMsg;
    sqlite3_free(errMsg);
    throw std::runtime_error("Database::execute failed: " + error);
  }
}

} // namespace QuickNotes::DB
