#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/elizas_list.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class ElizasListTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
    }
};

// ============================================================================
// Project Tests
// ============================================================================

TEST_F(ElizasListTest, ProjectCreation) {
    Project project;
    project.id = "proj-001";
    project.name = "Test Project";
    project.description = "A test project for unit testing";
    project.projectUrl = "https://example.com";
    project.github = "https://github.com/test/project";

    EXPECT_EQ(project.id, "proj-001");
    EXPECT_EQ(project.name, "Test Project");
    EXPECT_EQ(project.description, "A test project for unit testing");
    EXPECT_EQ(project.projectUrl, "https://example.com");
}

TEST_F(ElizasListTest, ProjectTags) {
    Project project;
    project.id = "proj-001";
    project.tags = {"ai", "agents", "eliza"};

    EXPECT_EQ(project.tags.size(), 3);
    EXPECT_THAT(project.tags, Contains("ai"));
    EXPECT_THAT(project.tags, Contains("agents"));
}

TEST_F(ElizasListTest, ProjectAuthor) {
    Project project;
    project.id = "proj-001";
    project.author.name = "Test Author";
    project.author.github = "testauthor";
    project.author.twitter = "testauthor_twitter";

    EXPECT_EQ(project.author.name, "Test Author");
    EXPECT_EQ(project.author.github, "testauthor");
    EXPECT_TRUE(project.author.twitter.has_value());
    EXPECT_EQ(project.author.twitter.value(), "testauthor_twitter");
}

TEST_F(ElizasListTest, ProjectMetrics) {
    Project project;
    project.id = "proj-001";
    project.metrics = Metrics{100, 25};

    EXPECT_TRUE(project.metrics.has_value());
    EXPECT_EQ(project.metrics.value().stars, 100);
    EXPECT_EQ(project.metrics.value().forks, 25);
}

// ============================================================================
// Collection Tests
// ============================================================================

TEST_F(ElizasListTest, CollectionCreation) {
    Collection collection;
    collection.id = "coll-001";
    collection.name = "AI Agents";
    collection.description = "Collection of AI agent projects";
    collection.featured = true;

    EXPECT_EQ(collection.id, "coll-001");
    EXPECT_EQ(collection.name, "AI Agents");
    EXPECT_TRUE(collection.featured);
}

TEST_F(ElizasListTest, CollectionWithProjects) {
    Collection collection;
    collection.id = "coll-001";
    collection.name = "AI Agents";
    collection.projects = {"proj-001", "proj-002", "proj-003"};

    EXPECT_EQ(collection.projects.size(), 3);
    EXPECT_THAT(collection.projects, Contains("proj-001"));
}

TEST_F(ElizasListTest, CollectionCurator) {
    Collection collection;
    collection.id = "coll-001";
    collection.curator.name = "Test Curator";
    collection.curator.github = "testcurator";

    EXPECT_EQ(collection.curator.name, "Test Curator");
    EXPECT_EQ(collection.curator.github, "testcurator");
}

// ============================================================================
// ElizasList Tests
// ============================================================================

TEST_F(ElizasListTest, ListCreation) {
    ElizasList list;

    EXPECT_EQ(list.getProjectCount(), 0);
    EXPECT_EQ(list.getCollectionCount(), 0);
}

TEST_F(ElizasListTest, AddProject) {
    ElizasList list;

    Project project;
    project.id = "proj-001";
    project.name = "Test Project";
    project.description = "A test project";

    bool added = list.addProject(project);
    EXPECT_TRUE(added);
    EXPECT_EQ(list.getProjectCount(), 1);
}

TEST_F(ElizasListTest, GetProject) {
    ElizasList list;

    Project project;
    project.id = "proj-001";
    project.name = "Test Project";
    project.description = "A test project";

    list.addProject(project);

    auto retrieved = list.getProject("proj-001");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved.value().name, "Test Project");
}

TEST_F(ElizasListTest, RemoveProject) {
    ElizasList list;

    Project project;
    project.id = "proj-001";
    project.name = "Test Project";

    list.addProject(project);
    EXPECT_EQ(list.getProjectCount(), 1);

    bool removed = list.removeProject("proj-001");
    EXPECT_TRUE(removed);
    EXPECT_EQ(list.getProjectCount(), 0);
}

TEST_F(ElizasListTest, UpdateProject) {
    ElizasList list;

    Project project;
    project.id = "proj-001";
    project.name = "Original Name";

    list.addProject(project);

    project.name = "Updated Name";
    bool updated = list.updateProject(project);
    EXPECT_TRUE(updated);

    auto retrieved = list.getProject("proj-001");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved.value().name, "Updated Name");
}

TEST_F(ElizasListTest, GetAllProjects) {
    ElizasList list;

    for (int i = 1; i <= 5; ++i) {
        Project project;
        project.id = "proj-" + std::to_string(i);
        project.name = "Project " + std::to_string(i);
        list.addProject(project);
    }

    auto projects = list.getAllProjects();
    EXPECT_EQ(projects.size(), 5);
}

TEST_F(ElizasListTest, GetProjectsByTag) {
    ElizasList list;

    Project project1;
    project1.id = "proj-001";
    project1.name = "AI Project";
    project1.tags = {"ai", "ml"};
    list.addProject(project1);

    Project project2;
    project2.id = "proj-002";
    project2.name = "Web Project";
    project2.tags = {"web", "frontend"};
    list.addProject(project2);

    Project project3;
    project3.id = "proj-003";
    project3.name = "AI Web Project";
    project3.tags = {"ai", "web"};
    list.addProject(project3);

    auto aiProjects = list.getProjectsByTag("ai");
    EXPECT_EQ(aiProjects.size(), 2);
}

TEST_F(ElizasListTest, GetProjectsByAuthor) {
    ElizasList list;

    Project project1;
    project1.id = "proj-001";
    project1.author.github = "author1";
    list.addProject(project1);

    Project project2;
    project2.id = "proj-002";
    project2.author.github = "author2";
    list.addProject(project2);

    Project project3;
    project3.id = "proj-003";
    project3.author.github = "author1";
    list.addProject(project3);

    auto author1Projects = list.getProjectsByAuthor("author1");
    EXPECT_EQ(author1Projects.size(), 2);
}

TEST_F(ElizasListTest, SearchProjects) {
    ElizasList list;

    Project project1;
    project1.id = "proj-001";
    project1.name = "AI Assistant";
    project1.description = "An AI-powered assistant";
    list.addProject(project1);

    Project project2;
    project2.id = "proj-002";
    project2.name = "Web App";
    project2.description = "A web application";
    list.addProject(project2);

    auto aiResults = list.searchProjects("AI");
    EXPECT_GE(aiResults.size(), 1);
}

// ============================================================================
// Collection Management Tests
// ============================================================================

TEST_F(ElizasListTest, AddCollection) {
    ElizasList list;

    Collection collection;
    collection.id = "coll-001";
    collection.name = "Featured Projects";

    bool added = list.addCollection(collection);
    EXPECT_TRUE(added);
    EXPECT_EQ(list.getCollectionCount(), 1);
}

TEST_F(ElizasListTest, GetCollection) {
    ElizasList list;

    Collection collection;
    collection.id = "coll-001";
    collection.name = "Featured Projects";

    list.addCollection(collection);

    auto retrieved = list.getCollection("coll-001");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved.value().name, "Featured Projects");
}

TEST_F(ElizasListTest, GetFeaturedCollections) {
    ElizasList list;

    Collection collection1;
    collection1.id = "coll-001";
    collection1.name = "Featured";
    collection1.featured = true;
    list.addCollection(collection1);

    Collection collection2;
    collection2.id = "coll-002";
    collection2.name = "Regular";
    collection2.featured = false;
    list.addCollection(collection2);

    auto featured = list.getFeaturedCollections();
    EXPECT_EQ(featured.size(), 1);
    EXPECT_EQ(featured[0].name, "Featured");
}

// ============================================================================
// Statistics Tests
// ============================================================================

TEST_F(ElizasListTest, GetProjectCount) {
    ElizasList list;

    for (int i = 1; i <= 10; ++i) {
        Project project;
        project.id = "proj-" + std::to_string(i);
        list.addProject(project);
    }

    EXPECT_EQ(list.getProjectCount(), 10);
}

TEST_F(ElizasListTest, GetAllTags) {
    ElizasList list;

    Project project1;
    project1.id = "proj-001";
    project1.tags = {"ai", "ml"};
    list.addProject(project1);

    Project project2;
    project2.id = "proj-002";
    project2.tags = {"web", "ai"};
    list.addProject(project2);

    auto tags = list.getAllTags();
    EXPECT_GE(tags.size(), 3);  // ai, ml, web (ai is deduplicated)
}

// ============================================================================
// JSON Serialization Tests
// ============================================================================

TEST_F(ElizasListTest, AuthorToJson) {
    Author author;
    author.name = "Test Author";
    author.github = "testauthor";
    author.twitter = "testauthor_twitter";

    nlohmann::json j = author;
    EXPECT_EQ(j["name"], "Test Author");
    EXPECT_EQ(j["github"], "testauthor");
}

TEST_F(ElizasListTest, AuthorFromJson) {
    nlohmann::json j = {
        {"name", "Test Author"},
        {"github", "testauthor"},
        {"twitter", "testauthor_twitter"}
    };

    Author author = j.get<Author>();
    EXPECT_EQ(author.name, "Test Author");
    EXPECT_EQ(author.github, "testauthor");
}

TEST_F(ElizasListTest, ProjectToJson) {
    Project project;
    project.id = "proj-001";
    project.name = "Test Project";
    project.description = "A test project";
    project.tags = {"ai", "ml"};

    nlohmann::json j = project;
    EXPECT_EQ(j["id"], "proj-001");
    EXPECT_EQ(j["name"], "Test Project");
}

TEST_F(ElizasListTest, ProjectFromJson) {
    nlohmann::json j = {
        {"id", "proj-001"},
        {"name", "Test Project"},
        {"description", "A test project"},
        {"projectUrl", "https://example.com"},
        {"github", "https://github.com/test/project"},
        {"image", ""},
        {"author", {{"name", "Author"}, {"github", "author"}}},
        {"donation", {{"transactionHash", ""}, {"amount", ""}, {"date", ""}}},
        {"tags", {"ai", "ml"}},
        {"addedOn", "2024-01-01"}
    };

    Project project = j.get<Project>();
    EXPECT_EQ(project.id, "proj-001");
    EXPECT_EQ(project.name, "Test Project");
    EXPECT_EQ(project.tags.size(), 2);
}

// ============================================================================
// Edge Cases Tests
// ============================================================================

TEST_F(ElizasListTest, AddDuplicateProject) {
    ElizasList list;

    Project project;
    project.id = "proj-001";
    project.name = "Original";

    list.addProject(project);

    project.name = "Duplicate";
    bool addedAgain = list.addProject(project);

    // Should fail or overwrite depending on implementation
    // Just verify we have exactly one project
    EXPECT_EQ(list.getProjectCount(), 1);
}

TEST_F(ElizasListTest, RemoveNonExistentProject) {
    ElizasList list;

    bool removed = list.removeProject("nonexistent");
    EXPECT_FALSE(removed);
}

TEST_F(ElizasListTest, GetNonExistentProject) {
    ElizasList list;

    auto project = list.getProject("nonexistent");
    EXPECT_FALSE(project.has_value());
}

TEST_F(ElizasListTest, EmptySearchResults) {
    ElizasList list;

    Project project;
    project.id = "proj-001";
    project.name = "Test";
    project.description = "Test project";
    list.addProject(project);

    auto results = list.searchProjects("nonexistent_query");
    EXPECT_EQ(results.size(), 0);
}
