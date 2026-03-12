#include "Editor.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <unistd.h>

namespace QuickNotes::Config::Editor {

std::string openEditor(const std::string &initialContent) {
  char tmpPath[] = "/tmp/QuickNotes_XXXXXX.md";
  int fd = mkstemps(tmpPath, 3);
  write(fd, initialContent.c_str(), initialContent.size());
  close(fd);
  endwin();
  const char *editor = std::getenv("EDITOR");
  if (!editor)
    editor = "vi";
  std::string cmd = std::string(editor) + " " + tmpPath;
  system(cmd.c_str());
  refresh();
  std::ifstream file(tmpPath);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  std::filesystem::remove(tmpPath);
  return content;
}
} // namespace QuickNotes::Config::Editor
