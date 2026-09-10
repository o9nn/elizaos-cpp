#include <gtest/gtest.h>

#include "elizaos/elizas_list.hpp"

#include <filesystem>
#include <fstream>
#include <string>

using namespace elizaos;

namespace {
class TempDir {
public:
    TempDir() {
        path = std::filesystem::temp_directory_path() /
               ("elizas_list_real_" + std::to_string(counter++));
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path);
    }
    ~TempDir() { std::filesystem::remove_all(path); }
    std::filesystem::path path;
    static inline int counter = 0;
};

Project completeProject() {
    Project value;
    value.id = "complete";
    value.name = "Complete";
    value.description = "All fields";
    value.projectUrl = "https://example.com/complete";
    value.github = "elizaos/complete";
    value.image = "https://example.com/complete.png";
    value.author = {"Alice", "alice", std::optional<std::string>("alice_ai")};
    value.donation = {"transaction01", "10.25", "2025-01-01"};
    value.tags = {"agent", "cpp"};
    value.addedOn = "2025-01-02";
    value.metrics = Metrics{42, 7};
    return value;
}

Collection completeCollection() {
    Collection value;
    value.id = "featured";
    value.name = "Featured";
    value.description = "All complete projects";
    value.projects = {"complete"};
    value.curator = {"Curator", "curator"};
    value.featured = true;
    return value;
}
}

TEST(ElizasListPersistence, AtomicRoundTripPreservesEveryFieldAndReplacesState) {
    TempDir temp;
    ElizasList source(temp.path);
    ASSERT_TRUE(source.addProject(completeProject()));
    ASSERT_TRUE(source.addCollection(completeCollection()));
    ASSERT_TRUE(source.saveToJson("nested/list.json"));

    ElizasList destination(temp.path);
    ASSERT_TRUE(destination.loadFromJson("nested/list.json"));
    ASSERT_EQ(destination.getProjectCount(), 1U);
    ASSERT_EQ(destination.getCollectionCount(), 1U);
    const auto value = destination.getProject("complete");
    ASSERT_TRUE(value);
    EXPECT_EQ(value->image, "https://example.com/complete.png");
    EXPECT_EQ(value->author.twitter, std::optional<std::string>("alice_ai"));
    EXPECT_EQ(value->donation.amount, "10.25");
    EXPECT_EQ(value->metrics->forks, 7);
    EXPECT_TRUE(destination.getCollection("featured")->featured);

    ElizasList empty(temp.path);
    ASSERT_TRUE(empty.saveToJson("empty.json"));
    ASSERT_TRUE(destination.loadFromJson("empty.json"));
    EXPECT_EQ(destination.getProjectCount(), 0U);
    EXPECT_EQ(destination.getCollectionCount(), 0U);
}

TEST(ElizasListPersistence, MalformedTruncatedTrailingWrongVersionAndUnknownFieldsRollBack) {
    TempDir temp;
    ElizasList list(temp.path);
    ASSERT_TRUE(list.addProject(completeProject()));
    ASSERT_TRUE(list.saveToJson("good.json"));
    const std::string good = [&] {
        std::ifstream input(temp.path / "good.json");
        return std::string(std::istreambuf_iterator<char>(input), {});
    }();

    const std::vector<std::pair<std::string, std::string>> cases = {
        {"malformed.json", "{"}, {"truncated.json", good.substr(0, good.size() / 2)},
        {"trailing.json", good + " trailing"},
        {"version.json", R"({"schema":"elizaos.elizas-list","version":2,"projects":[],"collections":[]})"},
        {"unknown.json", R"({"schema":"elizaos.elizas-list","version":1,"projects":[],"collections":[],"unknown":1})"}};
    for (const auto& item : cases) {
        std::ofstream(temp.path / item.first) << item.second;
        EXPECT_FALSE(list.loadFromJson(item.first)) << item.first;
        EXPECT_TRUE(list.getProject("complete").has_value()) << item.first;
    }
}

TEST(ElizasListPersistence, ProjectImportIsStrictTransactionalAndPreservesCollections) {
    TempDir temp;
    ElizasList list(temp.path);
    ASSERT_TRUE(list.addProject(completeProject()));
    ASSERT_TRUE(list.addCollection(completeCollection()));
    const auto exported = list.exportProjectsToJson();
    ElizasList other(temp.path);
    ASSERT_TRUE(other.loadProjectsFromJson(exported));
    EXPECT_TRUE(other.getProject("complete").has_value());

    EXPECT_FALSE(list.loadProjectsFromJson("{"));
    EXPECT_TRUE(list.getProject("complete").has_value());
    EXPECT_FALSE(list.loadProjectsFromJson(
        R"({"schema":"elizaos.elizas-list","version":1,"projects":[]})"));
    EXPECT_TRUE(list.getProject("complete").has_value());
}

TEST(ElizasListPersistence, PathsAreConfinedAndSymlinkOverwriteIsRejected) {
    TempDir temp;
    TempDir outside;
    ElizasList list(temp.path);
    ASSERT_TRUE(list.addProject(completeProject()));
    EXPECT_FALSE(list.saveToJson("../escape.json"));
    EXPECT_FALSE(list.loadFromJson(outside.path.string() + "/outside.json"));

    const auto victim = outside.path / "victim.json";
    std::ofstream(victim) << "do-not-overwrite";
    const auto link = temp.path / "link.json";
    std::filesystem::create_symlink(victim, link);
    EXPECT_FALSE(list.saveToJson("link.json"));
    EXPECT_FALSE(list.loadFromJson("link.json"));
    std::ifstream input(victim);
    EXPECT_EQ(std::string(std::istreambuf_iterator<char>(input), {}), "do-not-overwrite");

    const auto linkedDirectory = temp.path / "linked-directory";
    std::filesystem::create_directory_symlink(outside.path, linkedDirectory);
    EXPECT_FALSE(list.saveToJson("linked-directory/escape.json"));
}

TEST(ElizasListPersistence, FailedWritePreservesLastGoodSnapshotAndLeavesNoTempFiles) {
    TempDir temp;
    ElizasList list(temp.path);
    ASSERT_TRUE(list.addProject(completeProject()));
    ASSERT_TRUE(list.saveToJson("snapshot.json"));
    std::filesystem::permissions(temp.path, std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_exec, std::filesystem::perm_options::replace);
    const bool saved = list.saveToJson("new.json");
    std::filesystem::permissions(temp.path, std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
    if (!saved) { EXPECT_TRUE(std::filesystem::exists(temp.path / "snapshot.json")); }
    for (const auto& item : std::filesystem::directory_iterator(temp.path)) {
        EXPECT_EQ(item.path().filename().string().find(".tmp."), std::string::npos);
    }
}
