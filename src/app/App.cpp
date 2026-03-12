#include "app/App.hpp"
#include "app/state/MainMenuState.hpp"
#include "config/ConfigLoader.hpp"
#include "db/Database.hpp"
#include "db/NoteRepository.hpp"
#include "markdown/Colors.hpp"
#include <clocale>
#include <memory>
#include <ncurses.h>

namespace QuickNotes::App {

using namespace QuickNotes::App::State;

using Loader = QuickNotes::Config::ConfigLoader;

App::App()
    : m_config(std::make_shared<Config::Config>(Loader::load())), m_db(),
      m_repository(m_db) {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  if (can_change_color()) {
    Markdown::Colors::initColorPairs(*m_config);
  }
  wbkgd(stdscr, COLOR_PAIR(Markdown::Colors::PAIR_TEXT));
  auto initial =
      std::make_unique<MainMenuState>(stdscr, m_config, *this, m_repository);
  initial->onEnter();
  m_stateStack.push(std::move(initial));
};

App::~App() noexcept { endwin(); }

void App::run() {
  while (m_running) {
    m_stateStack.top()->render();
    int input = getch();
    if (auto next = m_stateStack.top()->handleInput(input)) {
      pushState(std::move(next));
    }
  }
}

void App::pushState(std::unique_ptr<AbstractState> state) {
  state->onEnter();
  m_stateStack.push(std::move(state));
}

void App::popState() {
  m_stateStack.top()->onExit();
  m_stateStack.pop();
  if (!m_stateStack.empty()) {
    m_stateStack.top()->render();
  }
}

} // namespace QuickNotes::App
