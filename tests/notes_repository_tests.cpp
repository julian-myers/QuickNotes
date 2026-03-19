#include "db/Database.hpp"
#include "db/NoteRepository.hpp"
#include <filesystem>
#include <gtest/gtest.h>

using namespace QuickNotes::DB;
using namespace QuickNotes::Model;

class NotesRepositoryTest : public ::testing::Test {
  protected:
    std::filesystem::path m_tempDir;
    std::unique_ptr<Database> m_db;
    std::unique_ptr<NotesRepository> m_repo;

    void SetUp() override {
        auto base = std::filesystem::temp_directory_path() / "qn_test_XXXXXX";
        std::string tmpl = base.string();
        char *dir = mkdtemp(tmpl.data());
        ASSERT_NE(dir, nullptr);
        m_tempDir = dir;
        setenv("XDG_DATA_HOME", m_tempDir.c_str(), 1);
        m_db = std::make_unique<Database>();
        m_repo = std::make_unique<NotesRepository>(*m_db);
    }

    void TearDown() override {
        m_repo.reset();
        m_db.reset();
        std::filesystem::remove_all(m_tempDir);
    }
};

// --- create ---

TEST_F(NotesRepositoryTest, CreateReturnsNoteWithCorrectTitle) {
    auto result = m_repo->create("My Note");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->title, "My Note");
}

TEST_F(NotesRepositoryTest, CreateAssignsId) {
    auto result = m_repo->create("My Note");
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result->id, 0);
}

TEST_F(NotesRepositoryTest, CreateSetsEmptyContent) {
    auto result = m_repo->create("My Note");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->content.empty());
}

TEST_F(NotesRepositoryTest, CreateDuplicateTitleReturnsError) {
    ASSERT_TRUE(m_repo->create("Duplicate").has_value());
    auto result = m_repo->create("Duplicate");
    EXPECT_FALSE(result.has_value());
}

// --- findById ---

TEST_F(NotesRepositoryTest, FindByIdReturnsNote) {
    auto created = m_repo->create("Find Me");
    ASSERT_TRUE(created.has_value());
    auto found = m_repo->findById(created->id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->title, "Find Me");
}

TEST_F(NotesRepositoryTest, FindByIdUnknownIdReturnsError) {
    auto result = m_repo->findById(999);
    EXPECT_FALSE(result.has_value());
}

// --- findAll ---

TEST_F(NotesRepositoryTest, FindAllEmptyWhenNoNotes) {
    EXPECT_TRUE(m_repo->findAll().empty());
}

TEST_F(NotesRepositoryTest, FindAllReturnsAllNotes) {
    ASSERT_TRUE(m_repo->create("Alpha").has_value());
    ASSERT_TRUE(m_repo->create("Beta").has_value());
    ASSERT_TRUE(m_repo->create("Gamma").has_value());
    EXPECT_EQ(m_repo->findAll().size(), 3u);
}

// --- update ---

TEST_F(NotesRepositoryTest, UpdateChangesContent) {
    auto note = m_repo->create("To Update");
    ASSERT_TRUE(note.has_value());
    note->content = "new content";
    auto updated = m_repo->update(*note);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->content, "new content");
}

TEST_F(NotesRepositoryTest, UpdateChangesTitle) {
    auto note = m_repo->create("Old Title");
    ASSERT_TRUE(note.has_value());
    note->title = "New Title";
    auto updated = m_repo->update(*note);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->title, "New Title");
}

// --- remove ---

TEST_F(NotesRepositoryTest, RemoveDeletesNote) {
    auto note = m_repo->create("To Delete");
    ASSERT_TRUE(note.has_value());
    auto removed = m_repo->remove(*note);
    ASSERT_TRUE(removed.has_value());
    EXPECT_FALSE(m_repo->findById(note->id).has_value());
}

TEST_F(NotesRepositoryTest, RemoveReturnsErrorForMissingNote) {
    Note ghost{999, "Ghost", "", "", ""};
    auto result = m_repo->remove(ghost);
    EXPECT_FALSE(result.has_value());
}

// --- findByTitle ---

TEST_F(NotesRepositoryTest, FindByTitleExactMatch) {
    ASSERT_TRUE(m_repo->create("Exact").has_value());
    auto results = m_repo->findByTitle("Exact");
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].title, "Exact");
}

TEST_F(NotesRepositoryTest, FindByTitlePartialMatch) {
    ASSERT_TRUE(m_repo->create("My Shopping List").has_value());
    ASSERT_TRUE(m_repo->create("My Todo List").has_value());
    ASSERT_TRUE(m_repo->create("Unrelated").has_value());
    auto results = m_repo->findByTitle("List");
    EXPECT_EQ(results.size(), 2u);
}

TEST_F(NotesRepositoryTest, FindByTitleNoMatchReturnsEmpty) {
    ASSERT_TRUE(m_repo->create("Something").has_value());
    auto results = m_repo->findByTitle("zzznomatch");
    EXPECT_TRUE(results.empty());
}

// --- findMostRecent ---

TEST_F(NotesRepositoryTest, FindMostRecentReturnsRequestedCount) {
    ASSERT_TRUE(m_repo->create("A").has_value());
    ASSERT_TRUE(m_repo->create("B").has_value());
    ASSERT_TRUE(m_repo->create("C").has_value());
    auto results = m_repo->findMostRecent(2);
    EXPECT_EQ(results.size(), 2u);
}

TEST_F(NotesRepositoryTest, FindMostRecentEmptyWhenNoNotes) {
    auto results = m_repo->findMostRecent(5);
    EXPECT_TRUE(results.empty());
}
