#pragma once

#include <memory>

namespace QuickNotes::App::State {
class AbstractState;
}

namespace QuickNotes::App {

class IAppController {
  public:
    virtual ~IAppController() = default;
    virtual void quit() = 0;
    virtual void pushState(std::unique_ptr<State::AbstractState> state) = 0;
    virtual void popState() = 0;
};

} // namespace QuickNotes::App
