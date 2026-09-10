#include <gtest/gtest.h>

#include "elizaos/registry.hpp"
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace elizaos;

namespace {
RegistryEntry entry(std::string name, std::string version = "1.0.0") {
    RegistryEntry value;
    value.name = std::move(name);
    value.repositoryUrl = "https://example.com/" + value.name + ".git";
    value.description = "Plugin " + value.name;
    value.version = std::move(version);
    value.author = "Eliza OS";
    value.tags = {"agent", "local"};
    value.metadata = {{"license", "MIT"}};
    return value;
}

std::string registryJson(const std::vector<RegistryEntry>& entries) {
    nlohmann::json root = {{"schema", "elizaos.registry"}, {"version", 1},
                           {"entries", nlohmann::json::array()}};
    for (const auto& value : entries) {
        root["entries"].push_back({
            {"name", value.name}, {"repositoryUrl", value.repositoryUrl},
            {"description", value.description}, {"version", value.version},
            {"author", value.author}, {"tags", value.tags},
            {"dependencies", value.dependencies}, {"metadata", value.metadata}});
    }
    return root.dump();
}

class TempDir {
public:
    TempDir() {
        path = std::filesystem::temp_directory_path() /
               ("registry_test_" + std::to_string(counter++));
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path);
    }
    ~TempDir() { std::filesystem::remove_all(path); }
    std::filesystem::path path;
    static inline int counter = 0;
};
}

TEST(RegistryConfig, DefaultsAreLocalAndBounded) {
    RegistryConfig config;
    EXPECT_FALSE(config.enableRemoteRegistry);
    EXPECT_TRUE(config.registryUrl.empty());
    EXPECT_GT(config.cacheTtlSeconds, 0);
    EXPECT_FALSE(config.cacheDirectory.empty());
}

TEST(Registry, RegistrationValidationAndConflictSemantics) {
    Registry registry;
    const auto alpha = entry("alpha", "1.2.3");
    EXPECT_TRUE(registry.registerEntry(alpha));
    EXPECT_TRUE(registry.registerEntry(alpha));
    EXPECT_EQ(registry.getPluginCount(), 1U);

    auto conflict = alpha;
    conflict.description = "different";
    EXPECT_FALSE(registry.registerEntry(conflict));
    EXPECT_EQ(registry.getPlugin("alpha")->description, alpha.description);

    auto invalid = entry("bad id");
    EXPECT_FALSE(registry.registerEntry(invalid));
    invalid = entry("bad-url");
    invalid.repositoryUrl = "file:///etc/passwd";
    EXPECT_FALSE(registry.registerEntry(invalid));
    invalid = entry("bad-version");
    invalid.version = "01.2.3";
    EXPECT_FALSE(registry.registerEntry(invalid));
    invalid = entry("bad-metadata");
    invalid.metadata = {{"bad key", "value"}};
    EXPECT_FALSE(registry.registerEntry(invalid));
    invalid = entry("bad-tags");
    invalid.tags = {"agent", "agent"};
    EXPECT_FALSE(registry.registerEntry(invalid));
}

TEST(Registry, DependenciesMustExistMatchExactlyAndRemainAcyclic) {
    Registry registry;
    EXPECT_TRUE(registry.registerEntry(entry("core", "2.0.0")));
    auto dependent = entry("dependent");
    dependent.dependencies = {{"core", "2.0.0"}};
    EXPECT_TRUE(registry.registerEntry(dependent));
    EXPECT_FALSE(registry.unregisterEntry("core"));
    EXPECT_TRUE(registry.unregisterEntry("dependent"));
    EXPECT_TRUE(registry.unregisterEntry("core"));

    auto missing = entry("missing-dependency");
    missing.dependencies = {{"absent", "1.0.0"}};
    EXPECT_FALSE(registry.registerEntry(missing));

    TempDir temp;
    auto a = entry("a");
    auto b = entry("b");
    a.dependencies = {{"b", "1.0.0"}};
    b.dependencies = {{"a", "1.0.0"}};
    const auto file = temp.path / "cycle.json";
    std::ofstream(file) << registryJson({a, b});
    EXPECT_FALSE(registry.loadLocalRegistry(file.string()));
    EXPECT_EQ(registry.getPluginCount(), 0U);
}

TEST(Registry, QueriesAreLiteralCaseInsensitiveAndDeterministic) {
    Registry registry;
    auto zeta = entry("zeta", "1.0.0");
    zeta.description = "C++ [agent]";
    auto alpha = entry("alpha", "2.0.0-alpha.1");
    alpha.author = "ALICE";
    auto beta = entry("beta", "2.0.0");
    ASSERT_TRUE(registry.registerEntry(zeta));
    ASSERT_TRUE(registry.registerEntry(alpha));
    ASSERT_TRUE(registry.registerEntry(beta));

    const auto all = registry.getAllPlugins();
    ASSERT_EQ(all.size(), 3U);
    EXPECT_EQ(all[0].name, "alpha");
    EXPECT_EQ(all[1].name, "beta");
    EXPECT_EQ(all[2].name, "zeta");
    EXPECT_EQ(registry.searchPlugins("alice").at(0).name, "alpha");
    EXPECT_EQ(registry.searchPlugins("[agent]").at(0).name, "zeta");
    EXPECT_EQ(registry.searchPlugins("").size(), 3U);

    const auto versions = registry.getPluginsSortedByVersion();
    ASSERT_EQ(versions.size(), 3U);
    EXPECT_EQ(versions[0].name, "beta");
    EXPECT_EQ(versions[1].name, "alpha");
    EXPECT_EQ(versions[2].name, "zeta");
}

TEST(Registry, SemanticVersionComparisonImplementsSemVerPrecedence) {
    EXPECT_LT(Registry::compareSemanticVersions("1.0.0-alpha", "1.0.0-alpha.1"), 0);
    EXPECT_LT(Registry::compareSemanticVersions("1.0.0-alpha.1", "1.0.0-alpha.beta"), 0);
    EXPECT_LT(Registry::compareSemanticVersions("1.0.0-beta.11", "1.0.0-rc.1"), 0);
    EXPECT_LT(Registry::compareSemanticVersions("1.0.0-rc.1", "1.0.0"), 0);
    EXPECT_EQ(Registry::compareSemanticVersions("1.0.0+one", "1.0.0+two"), 0);
    EXPECT_THROW(Registry::compareSemanticVersions("1.0", "1.0.0"), std::invalid_argument);
    EXPECT_THROW(Registry::compareSemanticVersions("1.0.0-01", "1.0.0"), std::invalid_argument);
}

TEST(Registry, StrictLoadRollsBackOnMalformedTruncatedTrailingAndUnknownFields) {
    TempDir temp;
    Registry registry;
    const auto good = temp.path / "good.json";
    std::ofstream(good) << registryJson({entry("kept")});
    ASSERT_TRUE(registry.loadLocalRegistry(good.string()));
    ASSERT_EQ(registry.getPluginCount(), 1U);
    EXPECT_FALSE(registry.getLastRefreshTime().empty());

    for (const auto& pair : std::vector<std::pair<std::string, std::string>>{
             {"malformed", "{"}, {"trailing", registryJson({entry("new")}) + " junk"},
             {"wrong-schema", R"({"schema":"other","version":1,"entries":[]})"},
             {"unknown", R"({"schema":"elizaos.registry","version":1,"entries":[],"extra":1})"}}) {
        const auto file = temp.path / (pair.first + ".json");
        std::ofstream(file) << pair.second;
        EXPECT_FALSE(registry.loadLocalRegistry(file.string())) << pair.first;
        EXPECT_TRUE(registry.getPlugin("kept").has_value()) << pair.first;
    }
}
