# QuickNotes

A fast, keyboard-driven note-taking TUI application. QuickNotes is built as a low-friction alternative to graphical editors like Obsidian. The application is completely contained inside the terminal, notes are stored in an SQLite database, and Markdown renders inline with syntax highlighting powered by a custom markdown parsing/rendering pipeline.

## Features

- **Markdown Rendering**: Currently handles headings, bold, italic, inline code, code blocks, block quotes, ordered/unordered lists, tables, and horizontal rules.
- **SQLite Persistence**: All notes are stored on a local, serverless SQLite database.
- **Fully keyboard-driven**: All keybindings are configurable.
- **External Editor integration**: Open any note in $EDITOR and changes will be saved back automatically upon exit of the editor.
- **TOML configuration**: Customize colors and keybindings in `$HOME/.config/QuickNotes/config.toml`.

---

## Building

### Prerequisites

- CMake >= 3.25
- A C++23 capable compiler.
- ncursesw
- SQLite3
- Internet access on first build.

### Build

``` bash
cmake -B build
cmake --build --build
```

### Run
./build/QuickNotes

---

## Configuration

```toml
[colors]
heading1    = "#CBA6F7"
heading2    = "#F38BA8"
foreground  = "#CDD6F4"
background  = "#1E1E2E"
bold_fg     = "#89B4FA"
italic_fg   = "#74C7EC"
# ... see full list below
 
[keybinds]
# keybinds not yet exposed via config — coming soon
```
<details>
<summary>Full color key reference</summary>
 
| Key | Description |
| :-- | :---------- |
| `heading1` – `heading6` | Heading level foreground colors |
| `heading_bg` | Heading background |
| `bold_fg` | Bold text foreground |
| `italic_fg` | Italic text foreground |
| `bold_italic_fg` | Bold-italic text foreground |
| `in_line_code_fg` / `in_line_code_bg` | Inline code colors |
| `code_block_fg` / `code_block_bg` | Code block colors |
| `horizontal_rule_fg` | Horizontal rule color |
| `block_quote_left_border_fg` | Block quote border color |
| `list_bullet_fg` / `list_number_fg` | List marker colors |
| `table_border_fg` / `table_header_fg` / `table_body_fg` | Table colors |
| `foreground` / `background` | Base text colors |
 
</details>
 
---

## Default Keybindings

| Key | Action |
| :-- | :----- |
| `j` / `k` | Move down / up |
| `h` / `l` | Move left / right |
| `n` | Open notes list (from main menu) |
| `a` | Add a new note |
| `d` | Delete selected note |
| `Enter` | Select / open |
| `e` | Open note in `$EDITOR` |
| `Esc` | Go back |
| `/` | Search |
| `q` | Quit |
 
---

## TODO (ordered by priority):

- [ ] Note renaming.
- [ ] Keybinding / more robust color configuration
- [x] Search / filter through notes.
- [ ] Fuzzy find within content of notes across all notes or within a single note.
- [ ] Add tags, categories, and favorites.
- [ ] Note list custom ordering.
- [ ] Parse/render links, arrows, and other markdown features (maybe not links).
- [ ] Tree-sitter syntax highlighting for code blocks.
- [ ] Importing/exporting notes.
- [ ] Unit tests for configuration loading.
- [ ] Unit Tests for the Database.
- [ ] Integration Tests for the States.

---

## License

MIT
