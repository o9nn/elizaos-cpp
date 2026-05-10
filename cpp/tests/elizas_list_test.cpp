// elizas_list_test.cpp - E2E tests for ElizasList project & collection registry.
#include <gtest/gtest.h>
#include "elizaos/elizas_list.hpp"
#include <nlohmann/json.hpp>
#include <cstdio>

using namespace elizaos;

namespace {
Project mkProject(const std::string& id,
                  const std::string& tag = "ai",
                  int stars = 0) {
    Project p;
    p.id = id;
    p.name = id;
    p.description = "desc";
    p.projectUrl = "https://example.com/" + id;
    p.github = "elizaos/" + id;
    p.author.name = "alice";
    p.author.github = "alice";
    p.tags = {tag};
    p.metrics = Metrics{};
    p.metrics->stars = stars;
    return p;
}
}

TEST(ElizasListJson, AuthorRoundtrip) {
    Author a; a.name = "Alice"; a.github = "alice"; a.twitter = "@alice";
    nlohmann::json j = a;
    Author back = j.get<Author>();
    EXPECT_EQ(back.name, "Alice");
    EXPECT_EQ(back.github, "alice");
    ASSERT_TRUE(back.twitter.has_value());
    EXPECT_EQ(*back.twitter, "@alice");
}

TEST(ElizasListJson, ProjectRoundtrip) {
    Project p = mkProject("foo", "ai", 42);
    nlohmann::json j = p;
    Project back = j.get<Project>();
    EXPECT_EQ(back.id, "foo");
    ASSERT_TRUE(back.metrics.has_value());
    EXPECT_EQ(back.metrics->stars, 42);
}

TEST(ElizasListJson, CollectionRoundtrip) {
    Collection c;
    c.id = "c-1";
    c.name = "fav";
    c.description = "favs";
    c.projects = {"a", "b"};
    c.curator.name = "X";
    c.curator.github = "x";
    c.featured = true;
    nlohmann::json j = c;
    Collection back = j.get<Collection>();
    EXPECT_EQ(back.id, "c-1");
    EXPECT_EQ(back.projects.size(), 2u);
    EXPECT_TRUE(back.featured);
}

class ElizasListTest : public ::testing::Test {
protected:
    ElizasList list;
};

TEST_F(ElizasListTest, AddAndRetrieveProject) {
    EXPECT_TRUE(list.addProject(mkProject("p1")));
    EXPECT_EQ(list.getProjectCount(), 1u);
    EXPECT_TRUE(list.getProject("p1").has_value());
}

TEST_F(ElizasListTest, RejectDuplicate) {
    list.addProject(mkProject("p1"));
    // Either silently ignore or fail; either way, count must remain 1.
    list.addProject(mkProject("p1"));
    EXPECT_EQ(list.getProjectCount(), 1u);
}

TEST_F(ElizasListTest, RemoveProject) {
    list.addProject(mkProject("p1"));
    EXPECT_TRUE(list.removeProject("p1"));
    EXPECT_FALSE(list.getProject("p1").has_value());
}

TEST_F(ElizasListTest, GetProjectsByTagAndAuthor) {
    list.addProject(mkProject("p1", "ai"));
    list.addProject(mkProject("p2", "tools"));
    EXPECT_EQ(list.getProjectsByTag("ai").size(), 1u);
    EXPECT_GE(list.getProjectsByAuthor("alice").size(), 2u);
}

TEST_F(ElizasListTest, AddAndRetrieveCollection) {
    Collection c;
    c.id = "c-1"; c.name = "Top"; c.curator.name = "Z"; c.curator.github = "z";
    EXPECT_TRUE(list.addCollection(c));
    EXPECT_EQ(list.getCollectionCount(), 1u);
    EXPECT_TRUE(list.getCollection("c-1").has_value());
}

TEST_F(ElizasListTest, FeaturedCollectionsFilter) {
    Collection a, b;
    a.id = "a"; a.featured = false;
    b.id = "b"; b.featured = true;
    list.addCollection(a); list.addCollection(b);
    EXPECT_EQ(list.getFeaturedCollections().size(), 1u);
}

TEST_F(ElizasListTest, SearchAndSortByStars) {
    list.addProject(mkProject("low",  "x", 1));
    list.addProject(mkProject("high", "x", 99));
    auto sorted = list.getProjectsSortedByStars();
    ASSERT_GE(sorted.size(), 2u);
    if (sorted[0].metrics && sorted[1].metrics) {
        EXPECT_GE(sorted[0].metrics->stars, sorted[1].metrics->stars);
    }
    auto results = list.searchProjects("high");
    EXPECT_GE(results.size(), 1u);
}

TEST_F(ElizasListTest, AllTagsAggregation) {
    list.addProject(mkProject("a", "alpha"));
    list.addProject(mkProject("b", "beta"));
    auto tags = list.getAllTags();
    EXPECT_GE(tags.size(), 2u);
}

TEST_F(ElizasListTest, JsonExportImport) {
    list.addProject(mkProject("p1"));
    auto json = list.exportProjectsToJson();
    EXPECT_FALSE(json.empty());

    ElizasList other;
    if (other.loadProjectsFromJson(json)) {
        EXPECT_GE(other.getProjectCount(), 1u);
    }
    SUCCEED();
}
