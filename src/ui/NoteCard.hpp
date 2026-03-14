#pragma once

#include "models/Notes.hpp"
#include "ui/Subwindow.hpp"
#include "ui/Widget.hpp"
#include <memory>

namespace QuickNotes::UI {

class NoteCard : public Widget {
  public:
    explicit NoteCard(Model::Note &note);
    void draw() override;

  private:
    std::unique_ptr<SubWindow> m_window;
    std::string m_title;
    const std::string m_date;
    std::string m_tags;
};

} // namespace QuickNotes::UI
