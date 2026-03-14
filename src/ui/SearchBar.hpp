#pragma once

#include "ui/Widget.hpp"
#include <string>
#include <string_view>

namespace QuickNotes::UI {

class SearchBar : public Widget {

  public:
    explicit SearchBar(WINDOW *parent);

    void draw() override;

    void setInputBuffer(std::string_view buffer);

    void setLabel(std::string_view label);

  private:
    std::string m_inputBuffer;
    std::string m_label;
};

} // namespace QuickNotes::UI
