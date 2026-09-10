#include <gtest/gtest.h>

#include "elizaos/registry.hpp"
#include <nlohmann/json.hpp>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <memory>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {
RegistryEntry makeEntry(const std::string& name, int version = 1) {
    RegistryEntry entry;
    entry.name = name;
    entry.repositoryUrl = "https://example.com/" + name + ".git";
    entry.description = "Description " + name;
    entry.version = std::to_string(version) + ".0.0";
    entry.author = "author";
    entry.tags = {"tag"};
    entry.metadata = {{"license", "MIT"}};
    return entry;
}

std::string oneEntryJson(const RegistryEntry& entry) {
    nlohmann::json root = {{"schema", "elizaos.registry"}, {"version", 1},
                           {"entries", nlohmann::json::array({{
                               {"name", entry.name}, {"repositoryUrl", entry.repositoryUrl},
                               {"description", entry.description}, {"version", entry.version},
                               {"author", entry.author}, {"tags", entry.tags},
                               {"dependencies", entry.dependencies}, {"metadata", entry.metadata}}})}};
    return root.dump();
}

class TempDir {
public:
    TempDir() {
        path = std::filesystem::temp_directory_path() /
               ("registry_comprehensive_" + std::to_string(counter++));
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path);
    }
    ~TempDir() { std::filesystem::remove_all(path); }
    std::filesystem::path path;
    static inline std::atomic<int> counter{0};
};
}

TEST(RegistryComprehensive, RefreshUsesOnlyFreshStrictLocalCacheWhenRemoteDisabled) {
    TempDir temp;
    RegistryConfig config;
    config.cacheDirectory = temp.path.string();
    config.enableRemoteRegistry = false;
    Registry registry(config);

    EXPECT_FALSE(registry.refreshRegistry().get());
    EXPECT_FALSE(registry.isRegistryCached());
    const auto cache = temp.path / "registry_index.json";
    std::ofstream(cache) << oneEntryJson(makeEntry("cached"));
    EXPECT_TRUE(registry.isRegistryCached());
    EXPECT_TRUE(registry.refreshRegistry().get());
    EXPECT_TRUE(registry.getPlugin("cached").has_value());

    std::ofstream(cache, std::ios::trunc) << "{";
    EXPECT_FALSE(registry.refreshRegistry().get());
    EXPECT_TRUE(registry.getPlugin("cached").has_value());
}

TEST(RegistryComprehensive, CacheSnapshotAtomicallyRoundTripsEveryFieldAndRejectsSymlink) {
    TempDir temp;
    RegistryConfig config;
    config.cacheDirectory = temp.path.string();
    Registry source(config);
    auto complete = makeEntry("complete", 3);
    complete.tags = {"agent", "cpp"};
    complete.metadata = {{"license", "MIT"}, {"runtime", "native"}};
    ASSERT_TRUE(source.registerEntry(complete));
    ASSERT_TRUE(source.saveCacheSnapshot());

    Registry destination(config);
    ASSERT_TRUE(destination.refreshRegistry().get());
    const auto restored = destination.getPlugin("complete");
    ASSERT_TRUE(restored);
    EXPECT_EQ(restored->repositoryUrl, complete.repositoryUrl);
    EXPECT_EQ(restored->version, "3.0.0");
    EXPECT_EQ(restored->tags, complete.tags);
    EXPECT_EQ(restored->metadata, complete.metadata);

    const auto cache = temp.path / "registry_index.json";
    const auto victim = temp.path / "victim.json";
    std::filesystem::remove(cache);
    std::ofstream(victim) << "unchanged";
    std::filesystem::create_symlink(victim, cache);
    EXPECT_FALSE(source.isRegistryCached());
    EXPECT_FALSE(source.saveCacheSnapshot());
    std::ifstream input(victim);
    EXPECT_EQ(std::string(std::istreambuf_iterator<char>(input), {}), "unchanged");
}

TEST(RegistryComprehensive, ConfigurationValidationAndSnapshotsAreSafe) {
    Registry registry;
    RegistryConfig config;
    config.cacheDirectory = "cache";
    config.cacheTtlSeconds = 42;
    EXPECT_TRUE(registry.trySetConfig(config));
    const auto snapshot = registry.getConfigSnapshot();
    EXPECT_EQ(snapshot.cacheTtlSeconds, 42);
    EXPECT_EQ(registry.getConfig().cacheDirectory, "cache");

    config.cacheTtlSeconds = 0;
    EXPECT_FALSE(registry.trySetConfig(config));
    EXPECT_THROW(registry.setConfig(config), std::invalid_argument);
    EXPECT_EQ(registry.getConfigSnapshot().cacheTtlSeconds, 42);

    config = RegistryConfig{};
    config.enableRemoteRegistry = true;
    config.registryUrl = "file:///tmp/not-remote";
    EXPECT_FALSE(registry.trySetConfig(config));
}

TEST(RegistryComprehensive, PluginRegistryReferenceHasStableOwningRegistry) {
    auto registry = std::make_shared<Registry>();
    const PluginRegistry* embedded = &registry->getPluginRegistry();
    const Registry& constant = *registry;
    EXPECT_EQ(&constant.getPluginRegistry(), embedded);
    EXPECT_TRUE(embedded->getActivePlugins().empty());
}

TEST(RegistryComprehensive, ConcurrentRegistrationDiscoveryQueriesAndStatistics) {
    Registry registry;
    constexpr int writers = 8;
    constexpr int perWriter = 100;
    std::atomic<bool> start{false};
    std::vector<std::thread> threads;
    for (int writer = 0; writer < writers; ++writer) {
        threads.emplace_back([&, writer] {
            while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
            for (int i = 0; i < perWriter; ++i) {
                EXPECT_TRUE(registry.registerEntry(makeEntry(
                    "plugin-" + std::to_string(writer) + "-" + std::to_string(i))));
                (void)registry.getPluginCount();
                (void)registry.searchPlugins("plugin");
                (void)registry.getAllPlugins();
            }
        });
    }
    start.store(true, std::memory_order_release);
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(registry.getPluginCount(), static_cast<std::size_t>(writers * perWriter));
    EXPECT_EQ(registry.getAllPlugins().front().name, "plugin-0-0");
}

TEST(RegistryComprehensive, SharedGlobalSnapshotSurvivesReplacementAndLegacyIsThreadPinned) {
    auto first = std::make_shared<Registry>();
    ASSERT_TRUE(first->registerEntry(makeEntry("first")));
    setGlobalRegistry(first);
    auto snapshot = getGlobalRegistryShared();
    Registry& legacy = getGlobalRegistry();
    EXPECT_EQ(&legacy, first.get());

    auto second = std::make_shared<Registry>();
    ASSERT_TRUE(second->registerEntry(makeEntry("second")));
    setGlobalRegistry(second);
    EXPECT_TRUE(snapshot->getPlugin("first").has_value());
    EXPECT_TRUE(legacy.getPlugin("first").has_value());
    EXPECT_TRUE(getGlobalRegistryShared()->getPlugin("second").has_value());

    setGlobalRegistry(std::unique_ptr<Registry>{});
    EXPECT_NE(getGlobalRegistryShared(), nullptr);
}

TEST(RegistryComprehensive, ConcurrentSingletonReplacementReturnsValidOwnedSnapshots) {
    std::atomic<bool> start{false};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&, i] {
            while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
            for (int round = 0; round < 100; ++round) {
                if (i % 2 == 0) setGlobalRegistry(std::make_shared<Registry>());
                auto snapshot = getGlobalRegistryShared();
                EXPECT_NE(snapshot, nullptr);
                (void)snapshot->getPluginCount();
            }
        });
    }
    start.store(true, std::memory_order_release);
    for (auto& thread : threads) thread.join();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
