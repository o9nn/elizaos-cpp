#include <gtest/gtest.h>

#include "elizaos/elizas_list.hpp"

#include <nlohmann/json.hpp>

using namespace elizaos;

namespace {
Project project(std::string id, int stars = 0, std::string date = "2025-01-01") {
    Project value;
    value.id = std::move(id);
    value.name = "Project " + value.id;
    value.description = "Local AI agent";
    value.projectUrl = "https://example.com/" + value.id;
    value.github = "elizaos/" + value.id;
    value.image = "https://example.com/images/" + value.id + ".png";
    value.author = {"Alice", "alice", std::optional<std::string>("alice_ai")};
    value.donation = {"transaction01", "1.25", "2025-01-02"};
    value.tags = {"agent", "local"};
    value.addedOn = std::move(date);
    value.metrics = Metrics{stars, stars / 2};
    return value;
}

Collection collection(std::string id, std::vector<std::string> projects = {}, bool featured = false) {
    Collection value;
    value.id = std::move(id);
    value.name = "Collection " + value.id;
    value.description = "Curated local projects";
    value.projects = std::move(projects);
    value.curator = {"Curator", "curator"};
    value.featured = featured;
    return value;
}
}

TEST(ElizasListJson, EveryPublicModelStrictlyRoundTripsAllFields) {
    Author author{"Alice", "alice", std::optional<std::string>("alice_ai")};
    Donation donation{"transaction01", "2.50", "2025-01-01"};
    Metrics metrics{10, 3};
    Project original = project("alpha", 10);
    Collection originalCollection = collection("featured", {"alpha"}, true);
    Curator curator{"Curator", "curator"};

    EXPECT_EQ(nlohmann::json(author).get<Author>().twitter, author.twitter);
    EXPECT_EQ(nlohmann::json(donation).get<Donation>().amount, donation.amount);
    EXPECT_EQ(nlohmann::json(metrics).get<Metrics>().forks, metrics.forks);
    const auto projectRoundtrip = nlohmann::json(original).get<Project>();
    EXPECT_EQ(projectRoundtrip.donation.transactionHash, original.donation.transactionHash);
    EXPECT_EQ(projectRoundtrip.metrics->stars, 10);
    EXPECT_EQ(nlohmann::json(curator).get<Curator>().github, curator.github);
    EXPECT_EQ(nlohmann::json(originalCollection).get<Collection>().projects,
              originalCollection.projects);

    auto invalid = nlohmann::json(original);
    invalid["unknown"] = true;
    EXPECT_THROW((void)invalid.get<Project>(), nlohmann::json::exception);
    invalid = nlohmann::json(original);
    invalid["metrics"]["stars"] = -1;
    EXPECT_THROW((void)invalid.get<Project>(), nlohmann::json::exception);
}

TEST(ElizasList, ProjectCrudRejectsInvalidAndConflictingDuplicates) {
    ElizasList list;
    auto alpha = project("alpha");
    EXPECT_TRUE(list.addProject(alpha));
    EXPECT_TRUE(list.addProject(alpha));
    EXPECT_EQ(list.getProjectCount(), 1U);

    auto conflict = alpha;
    conflict.name = "Conflict";
    EXPECT_FALSE(list.addProject(conflict));
    EXPECT_EQ(list.getProject("alpha")->name, alpha.name);
    EXPECT_TRUE(list.updateProject(conflict));
    EXPECT_EQ(list.getProject("alpha")->name, "Conflict");
    EXPECT_FALSE(list.updateProject(project("missing")));

    auto invalid = project("bad id");
    EXPECT_FALSE(list.addProject(invalid));
    invalid = project("bad-url");
    invalid.projectUrl = "file:///tmp/project";
    EXPECT_FALSE(list.addProject(invalid));
    invalid = project("bad-date");
    invalid.addedOn = "2025-02-30";
    EXPECT_FALSE(list.addProject(invalid));
    invalid = project("bad-tags");
    invalid.tags = {"agent", "agent"};
    EXPECT_FALSE(list.addProject(invalid));
    invalid = project("bad-donation");
    invalid.donation = {"onlyhash01", "", ""};
    EXPECT_FALSE(list.addProject(invalid));

    EXPECT_TRUE(list.removeProject("alpha"));
    EXPECT_FALSE(list.removeProject("alpha"));
}

TEST(ElizasList, CollectionsRequireExistingUniqueProjectsAndProtectReferences) {
    ElizasList list;
    ASSERT_TRUE(list.addProject(project("alpha")));
    ASSERT_TRUE(list.addProject(project("beta")));
    auto curated = collection("curated", {"alpha", "beta"}, true);
    EXPECT_TRUE(list.addCollection(curated));
    EXPECT_TRUE(list.addCollection(curated));
    EXPECT_EQ(list.getCollectionCount(), 1U);

    auto conflict = curated;
    conflict.featured = false;
    EXPECT_FALSE(list.addCollection(conflict));
    auto missing = collection("missing", {"absent"});
    EXPECT_FALSE(list.addCollection(missing));
    auto duplicate = collection("duplicate", {"alpha", "alpha"});
    EXPECT_FALSE(list.addCollection(duplicate));
    EXPECT_FALSE(list.removeProject("alpha"));

    curated.projects = {"beta"};
    EXPECT_TRUE(list.updateCollection(curated));
    EXPECT_TRUE(list.removeProject("alpha"));
    EXPECT_TRUE(list.removeCollection("curated"));
    EXPECT_FALSE(list.removeCollection("curated"));
}

TEST(ElizasList, FilteringSearchAndOrderAreDeterministic) {
    ElizasList list;
    auto zeta = project("zeta", 5, "2025-01-03");
    zeta.author.github = "bob";
    zeta.author.name = "Bob";
    zeta.tags = {"tools"};
    auto alpha = project("alpha", 10, "2025-01-02");
    alpha.description = "C++ [agent]";
    auto beta = project("beta", 10, "2025-01-02");
    ASSERT_TRUE(list.addProject(zeta));
    ASSERT_TRUE(list.addProject(alpha));
    ASSERT_TRUE(list.addProject(beta));

    EXPECT_EQ(list.getAllProjects().at(0).id, "alpha");
    EXPECT_EQ(list.getProjectsByTag("agent").at(0).id, "alpha");
    EXPECT_EQ(list.getProjectsByAuthor("alice").size(), 2U);
    EXPECT_EQ(list.searchProjects("[AGENT]").at(0).id, "alpha");
    EXPECT_EQ(list.searchProjects("").size(), 3U);

    const auto byStars = list.getProjectsSortedByStars();
    EXPECT_EQ(byStars[0].id, "alpha");
    EXPECT_EQ(byStars[1].id, "beta");
    const auto recent = list.getRecentProjects(2);
    EXPECT_EQ(recent[0].id, "zeta");
    EXPECT_EQ(recent[1].id, "alpha");
    EXPECT_TRUE(list.getRecentProjects(0).empty());
    EXPECT_TRUE(list.getRecentProjects(-1).empty());
    EXPECT_EQ(list.getAllTags(), (std::vector<std::string>{"agent", "local", "tools"}));
}

TEST(ElizasList, CollectionQueriesAreSortedAndFeatured) {
    ElizasList list;
    ASSERT_TRUE(list.addCollection(collection("zeta", {}, false)));
    ASSERT_TRUE(list.addCollection(collection("alpha", {}, true)));
    const auto all = list.getAllCollections();
    ASSERT_EQ(all.size(), 2U);
    EXPECT_EQ(all[0].id, "alpha");
    EXPECT_EQ(all[1].id, "zeta");
    ASSERT_EQ(list.getFeaturedCollections().size(), 1U);
    EXPECT_EQ(list.getFeaturedCollections()[0].id, "alpha");
    EXPECT_TRUE(list.getCollection("alpha").has_value());
    EXPECT_FALSE(list.getCollection("missing").has_value());
}
