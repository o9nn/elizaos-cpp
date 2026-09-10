#include <gtest/gtest.h>

#include "elizaos/elizas_list.hpp"

#include <filesystem>

using namespace elizaos;

namespace {
Project validProject(const std::string& id) {
    Project value;
    value.id = id;
    value.name = "Name";
    value.description = "Description";
    value.projectUrl = "https://example.com/" + id;
    value.github = "owner/" + id;
    value.author = {"Owner", "owner", std::nullopt};
    value.addedOn = "2025-01-01";
    return value;
}

Collection validCollection(const std::string& id) {
    Collection value;
    value.id = id;
    value.name = "Name";
    value.description = "Description";
    value.curator = {"Owner", "owner"};
    return value;
}
}

TEST(ElizasListUnit, EmptyStateAllAccessorsAreDefined) {
    ElizasList list;
    EXPECT_EQ(list.getProjectCount(), 0U);
    EXPECT_EQ(list.getCollectionCount(), 0U);
    EXPECT_TRUE(list.getAllProjects().empty());
    EXPECT_TRUE(list.getAllCollections().empty());
    EXPECT_TRUE(list.getProjectsByTag("tag").empty());
    EXPECT_TRUE(list.getProjectsByAuthor("owner").empty());
    EXPECT_TRUE(list.searchProjects("query").empty());
    EXPECT_TRUE(list.getProjectsSortedByStars().empty());
    EXPECT_TRUE(list.getRecentProjects().empty());
    EXPECT_TRUE(list.getFeaturedCollections().empty());
    EXPECT_TRUE(list.getAllTags().empty());
    EXPECT_FALSE(list.getProject("missing"));
    EXPECT_FALSE(list.getCollection("missing"));
    EXPECT_FALSE(list.removeProject("missing"));
    EXPECT_FALSE(list.removeCollection("missing"));
    EXPECT_FALSE(list.updateProject(validProject("missing")));
    EXPECT_FALSE(list.updateCollection(validCollection("missing")));
}

TEST(ElizasListUnit, PersistenceRootIsAbsoluteAndStable) {
    const auto relative = std::filesystem::path("relative-list-root");
    ElizasList list(relative);
    EXPECT_TRUE(list.getPersistenceRoot().is_absolute());
    EXPECT_EQ(list.getPersistenceRoot().filename(), relative.filename());
}

TEST(ElizasListUnit, ProjectSnapshotsDoNotExposeMutableStorage) {
    ElizasList list;
    ASSERT_TRUE(list.addProject(validProject("alpha")));
    auto snapshot = list.getProject("alpha");
    ASSERT_TRUE(snapshot);
    snapshot->name = "mutated copy";
    EXPECT_EQ(list.getProject("alpha")->name, "Name");
    auto all = list.getAllProjects();
    all[0].name = "also a copy";
    EXPECT_EQ(list.getProject("alpha")->name, "Name");
}

TEST(ElizasListUnit, CollectionSnapshotsDoNotExposeMutableStorage) {
    ElizasList list;
    ASSERT_TRUE(list.addCollection(validCollection("alpha")));
    auto snapshot = list.getCollection("alpha");
    ASSERT_TRUE(snapshot);
    snapshot->name = "mutated copy";
    EXPECT_EQ(list.getCollection("alpha")->name, "Name");
}
