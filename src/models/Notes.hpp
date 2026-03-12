#pragma once

#include <string>

namespace QuickNotes::Model {

struct Note {
    int id;
    std::string title;
    std::string content;
    std::string createdAt;
    std::string updatedAt;
};

} // namespace QuickNotes::Model
