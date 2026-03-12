#pragma once

#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <memory>

namespace QuickNotes::UI {

using ConfigPtr = std::shared_ptr<Config::Config>;

class NoteViewWidget : public Widget {

  public:
    explicit NoteViewWidget(
        WINDOW *window, ConfigPtr config, Model::Note &note
    );
    ~NoteViewWidget();
    void draw() override;

  private:
    ConfigPtr m_config;
    Model::Note m_note;
};

} // namespace QuickNotes::UI
