#pragma once

#include "Widget.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/NoteListWidget.hpp"
#include "ui/PreviewWidget.hpp"
#include "ui/SearchBar.hpp"
#include "ui/StatusBar.hpp"
#include "ui/Subwindow.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace QuickNotes::UI {

class NoteContainer : public Widget {

  public:
    explicit NoteContainer(
        WINDOW *window, std::shared_ptr<const Config::Config> config
    );

    void draw(const std::vector<Model::Note> &notes, int selectedIndex);
    void draw() override;
    void resize(WINDOW *window) override;
    void setMode(std::string_view mode);

  private:
    std::string m_modeLabel = "--- NORMAL ----";

    std::shared_ptr<const Config::Config> m_config;
    std::unique_ptr<NoteListWidget> m_list;
    std::unique_ptr<PreviewWidget> m_preview;
    std::unique_ptr<StatusBar> m_statusBar;
    std::unique_ptr<SearchBar> m_searchBar;
    SubWindow m_listWindow;
    SubWindow m_previewWindow;
    SubWindow m_statusWindow;
    SubWindow m_searchWindow;

    static Rect listRect(WINDOW *window);
    static Rect previewRect(WINDOW *window);
    static Rect searchBarRect(WINDOW *window);
    static Rect statusBarRect(WINDOW *window);
};

} // namespace QuickNotes::UI
