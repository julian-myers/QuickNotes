# Design

## Architecture
 
QuickNotes is structured around three main subsystems.
 
### Markdown Pipeline
 
Markdown content is processed through a classic three-stage pipeline:
 
```
Source string → Lexer → Token stream → Parser → AST → Renderer → ncurses window
```
 
- **Lexer** (`Lexer.hpp/cpp`) — A state-machine tokenizer with three states: `DEFAULT`, `IN_CODE_BLOCK`, and `IN_TABLE`. Produces a flat token stream.
- **Parser** (`Parser.hpp/cpp`) — A recursive descent parser driven by a dispatch table that maps block-level token types to parse functions. Produces a typed AST rooted at `DocumentNode`.
- **AST** (`AST.hpp`) — A set of node structs (`HeadingNode`, `CodeBlockNode`, `TableNode`, etc.) built on an `IVisitor` interface. New operations (export, search indexing) can be added without touching the node types.
- **Renderer** (`Renderer.hpp/cpp`) — Implements `IVisitor` and walks the AST, writing styled output to an ncurses `WINDOW` using color pairs initialized from the user's config.
 
### Application State Machine
 
App flow is managed by a stack-based state machine:
 
- `App` owns a `std::stack<std::unique_ptr<AbstractState>>` and a main loop that calls `render()` and `handleInput()` on the top state each frame.
- States communicate transitions by returning a `std::unique_ptr<AbstractState>` from `handleInput()`. Returning `nullptr` means stay in the current state.
- `IAppController` is an interface that exposes `quit()`, `pushState()`, and `popState()` to states, keeping them decoupled from the concrete `App` class.
 
Current states:
 
| State | Description |
| :---- | :---------- |
| `MainMenuState` | Title screen with Notes / Settings / Quit options |
| `NoteListState` | Sidebar note list + live Markdown preview panel |
| `NewNoteState` | Dialog for entering a new note title |
| `ViewingState` | Full-screen scrollable note view with editor launch |
| `DeleteNoteState` | Confirmation dialog for note deletion |
 
### Database Layer
 
- `Database` — RAII wrapper around a `sqlite3*`. Opens or creates the database at `$XDG_DATA_HOME/QuickNotes/notes.db` on construction, creates the schema if needed, and closes the connection in its destructor.
- `NotesRepository` — Typed CRUD interface over the `notes` table. All single-result operations return `std::expected<Model::Note, std::string>` (C++23). Prepared statements are managed by a `Statement` RAII struct that calls `sqlite3_finalize` on destruction.
 
