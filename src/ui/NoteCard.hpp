#pragma once

#include "models/Notes.hpp"
#include "ui/Subwindow.hpp"
#include "ui/Widget.hpp"

namespace QuickNotes::UI {

class NoteCard : public Widget {
  public:
    explicit NoteCard(WINDOW *parent, Rect rect, Model::Note note);
    void draw(bool selected);
    void draw() override;

    static constexpr int HEIGHT = 5;

  private:
    SubWindow m_subWin;
    Model::Note m_note;

    static std::string shortDate(const std::string &date);
};

} // namespace QuickNotes::UI
