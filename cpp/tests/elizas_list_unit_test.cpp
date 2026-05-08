// elizas_list_unit_test.cpp - Pure unit tests for individual ElizasList APIs.
#include <gtest/gtest.h>
#include "elizaos/elizas_list.hpp"
#include <nlohmann/json.hpp>

using namespace elizaos;

namespace {
Project mkProject(const std::string& id) {
    Project p;
    p.id = id;
    p.name = id;
    p.author.name = "x";
    p.author.github = "x";
    return p;
}
Collection mkCollection(const std::string& id, bool featured = false) {
    Collection c;
    c.id = id;
    c.name = id;
    c.curator.name = "y";
    c.curator.github = "y";
    c.featured = featured;
    return c;
}
}

TEST(ElizasListUnit, AddRemoveProject) {
    ElizasList l;
    EXPECT_TRUE(l.addProject(mkProject("p")));
    EXPECT_EQ(l.getProjectCount(), 1u);
    EXPECT_TRUE(l.removeProject("p"));
    EXPECT_EQ(l.getProjectCount(), 0u);
}

TEST(ElizasListUnit, RemoveMissingProjectReturnsFalse) {
    ElizasList l;
    EXPECT_FALSE(l.removeProject("nope"));
}

TEST(ElizasListUnit, UpdateProject) {
    ElizasList l;
    auto p = mkProject("p");
    l.addProject(p);
    p.description = "updated";
    EXPECT_TRUE(l.updateProject(p));
    auto got = l.getProject("p");
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->description, "updated");
}

TEST(ElizasListUnit, AddRemoveCollection) {
    ElizasList l;
    EXPECT_TRUE(l.addCollection(mkCollection("c")));
    EXPECT_EQ(l.getCollectionCount(), 1u);
    EXPECT_TRUE(l.removeCollection("c"));
}

TEST(ElizasListUnit, FeaturedFilter) {
    ElizasList l;
    l.addCollection(mkCollection("a", false));
    l.addCollection(mkCollection("b", true));
    EXPECT_EQ(l.getFeaturedCollections().size(), 1u);
}

TEST(ElizasListUnit, RecentProjectsRespectsLimit) {
    ElizasList l;
    for (int i = 0; i < 5; ++i) l.addProject(mkProject("p" + std::to_string(i)));
    EXPECT_LE(l.getRecentProjects(3).size(), 3u);
}
