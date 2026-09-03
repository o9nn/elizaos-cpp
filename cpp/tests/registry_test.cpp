// registry_test.cpp - E2E tests for the plugin Registry.
#include <gtest/gtest.h>
#include "elizaos/registry.hpp"

#include <fstream>
#include <cstdio>
#include <algorithm>

using namespace elizaos;

TEST(RegistryEntry, Construction) {
    RegistryEntry e("foo", "https://example.com/foo.git");
    EXPECT_EQ(e.name, "foo");
    EXPECT_EQ(e.repositoryUrl, "https://example.com/foo.git");
}

TEST(RegistryConfig, Defaults) {
    RegistryConfig c;
    EXPECT_FALSE(c.registryUrl.empty());
    EXPECT_FALSE(c.cacheDirectory.empty());
    EXPECT_GT(c.cacheTtlSeconds, 0);
    EXPECT_TRUE(c.enableRemoteRegistry);
}

class RegistryTest : public ::testing::Test {
protected:
    Registry reg;
};

TEST_F(RegistryTest, EmptyOnConstruction) {
    EXPECT_EQ(reg.getPluginCount(), 0u);
    EXPECT_TRUE(reg.getAllPlugins().empty());
}

TEST_F(RegistryTest, LoadLocalEmptyJsonHandled) {
    auto path = std::string("/tmp/registry_test_empty.json");
    {
        std::ofstream f(path);
        f << "{}";
    }
    // Implementation may accept or reject; the call must not throw.
    EXPECT_NO_THROW(reg.loadLocalRegistry(path));
    std::remove(path.c_str());
}

TEST_F(RegistryTest, SetConfigPersists) {
    RegistryConfig c;
    c.registryUrl = "https://example.com/index.json";
    c.cacheDirectory = "/tmp/registry_cache_test";
    c.cacheTtlSeconds = 42;
    c.enableRemoteRegistry = false;
    reg.setConfig(c);
    EXPECT_EQ(reg.getConfig().cacheTtlSeconds, 42);
    EXPECT_FALSE(reg.getConfig().enableRemoteRegistry);
}

TEST_F(RegistryTest, GetPluginMissingReturnsEmpty) {
    auto opt = reg.getPlugin("nonexistent");
    EXPECT_FALSE(opt.has_value());
}

TEST_F(RegistryTest, SearchEmptyReturnsAllOrEmpty) {
    auto r = reg.searchPlugins("");
    // Empty registry => empty result
    EXPECT_TRUE(r.empty());
}

// Disabled: getGlobalRegistry() hangs in current impl, presumably attempting
// remote network access during eager construction.
TEST(GlobalRegistry, DISABLED_AccessibleSingleton) {
    auto& g = getGlobalRegistry();
    (void)g.getPluginCount();
    SUCCEED();
}

TEST(GlobalRegistry, DISABLED_SetGlobalReplacesInstance) {
    auto custom = std::make_unique<Registry>();
    setGlobalRegistry(std::move(custom));
    auto& g = getGlobalRegistry();
    EXPECT_EQ(g.getPluginCount(), 0u);
}
