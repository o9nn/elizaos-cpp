// eliza_paths_test.cpp - Tests for environment-aware path resolution
#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <string>

#ifdef _WIN32
#include <stdlib.h>
inline int setenv(const char* name, const char* value, int /*overwrite*/) {
    return _putenv_s(name, value);
}
inline int unsetenv(const char* name) {
    return _putenv_s(name, "");
}
#else
#include <unistd.h>
#endif

namespace elizaos {
namespace eliza {
std::string getElizaDataPath();
std::string getElizaConfigPath();
std::string getElizaLogsPath();
std::string getElizaCharactersPath();
std::string getElizaKnowledgePath();
std::string getElizaPluginsPath();
std::string getElizaStatePath();
void resetPathCache();
} // namespace eliza
} // namespace elizaos

namespace {

class ScopedEnv {
public:
    explicit ScopedEnv(const char* name) : name_(name) {
        if (const char* value = std::getenv(name)) {
            had_value_ = true;
            old_value_ = value;
        }
    }
    ~ScopedEnv() {
        if (had_value_) {
            setenv(name_, old_value_.c_str(), 1);
        } else {
            unsetenv(name_);
        }
    }
    void set(const std::filesystem::path& value) const {
        setenv(name_, value.string().c_str(), 1);
    }
    void unset() const {
        unsetenv(name_);
    }
private:
    const char* name_;
    bool had_value_{false};
    std::string old_value_;
};

class ElizaPathsTest : public ::testing::Test {
protected:
    void SetUp() override {
        base_ = std::filesystem::temp_directory_path() / "elizaos_paths_test" / std::to_string(::getpid());
        std::filesystem::remove_all(base_);
        elizaos::eliza::resetPathCache();
    }
    void TearDown() override {
        elizaos::eliza::resetPathCache();
        std::filesystem::remove_all(base_);
    }
    std::filesystem::path base_;
};

} // namespace

TEST_F(ElizaPathsTest, UsesExplicitRootDirAndCreatesSubdirectories) {
    ScopedEnv rootDir("ELIZAOS_ROOT_DIR");
    ScopedEnv dataDir("ELIZAOS_DATA_DIR");
    dataDir.unset();
    rootDir.set(base_ / "root");

    const auto data = std::filesystem::path(elizaos::eliza::getElizaDataPath());
    const auto config = std::filesystem::path(elizaos::eliza::getElizaConfigPath());
    const auto logs = std::filesystem::path(elizaos::eliza::getElizaLogsPath());
    const auto chars = std::filesystem::path(elizaos::eliza::getElizaCharactersPath());
    const auto knowledge = std::filesystem::path(elizaos::eliza::getElizaKnowledgePath());
    const auto plugins = std::filesystem::path(elizaos::eliza::getElizaPluginsPath());
    const auto state = std::filesystem::path(elizaos::eliza::getElizaStatePath());

    EXPECT_EQ(data, (base_ / "root").lexically_normal());
    EXPECT_EQ(config, (base_ / "root" / "config").lexically_normal());
    EXPECT_EQ(logs, (base_ / "root" / "logs").lexically_normal());
    EXPECT_EQ(chars, (base_ / "root" / "characters").lexically_normal());
    EXPECT_EQ(knowledge, (base_ / "root" / "knowledge").lexically_normal());
    EXPECT_EQ(plugins, (base_ / "root" / "plugins").lexically_normal());
    EXPECT_EQ(state, (base_ / "root" / "state").lexically_normal());

    // Verify directories were actually created
    EXPECT_TRUE(std::filesystem::is_directory(data));
    EXPECT_TRUE(std::filesystem::is_directory(config));
    EXPECT_TRUE(std::filesystem::is_directory(logs));
    EXPECT_TRUE(std::filesystem::is_directory(chars));
    EXPECT_TRUE(std::filesystem::is_directory(knowledge));
    EXPECT_TRUE(std::filesystem::is_directory(plugins));
    EXPECT_TRUE(std::filesystem::is_directory(state));
}

TEST_F(ElizaPathsTest, UsesExplicitDataDirWhenRootNotSet) {
    ScopedEnv rootDir("ELIZAOS_ROOT_DIR");
    ScopedEnv dataDir("ELIZAOS_DATA_DIR");
    rootDir.unset();
    dataDir.set(base_ / "data");

    const auto data = std::filesystem::path(elizaos::eliza::getElizaDataPath());
    EXPECT_EQ(data, (base_ / "data").lexically_normal());
    EXPECT_TRUE(std::filesystem::is_directory(data));
}

TEST_F(ElizaPathsTest, RootDirTakesPrecedenceOverDataDir) {
    ScopedEnv rootDir("ELIZAOS_ROOT_DIR");
    ScopedEnv dataDir("ELIZAOS_DATA_DIR");
    rootDir.set(base_ / "root_priority");
    dataDir.set(base_ / "data_ignored");

    const auto data = std::filesystem::path(elizaos::eliza::getElizaDataPath());
    EXPECT_EQ(data, (base_ / "root_priority").lexically_normal());
}

TEST_F(ElizaPathsTest, FallsBackToCwdEliza) {
    ScopedEnv rootDir("ELIZAOS_ROOT_DIR");
    ScopedEnv dataDir("ELIZAOS_DATA_DIR");
    rootDir.unset();
    dataDir.unset();

    const auto data = std::filesystem::path(elizaos::eliza::getElizaDataPath());
    const auto expected = (std::filesystem::current_path() / ".eliza").lexically_normal();
    EXPECT_EQ(data, expected);
}

TEST_F(ElizaPathsTest, RepeatedCallsReturnStablePaths) {
    ScopedEnv rootDir("ELIZAOS_ROOT_DIR");
    ScopedEnv dataDir("ELIZAOS_DATA_DIR");
    rootDir.set(base_ / "stable");
    dataDir.unset();

    const auto first = elizaos::eliza::getElizaDataPath();
    const auto second = elizaos::eliza::getElizaDataPath();
    EXPECT_EQ(first, second);
}

TEST_F(ElizaPathsTest, ResetCacheAllowsReResolution) {
    ScopedEnv rootDir("ELIZAOS_ROOT_DIR");
    ScopedEnv dataDir("ELIZAOS_DATA_DIR");
    rootDir.set(base_ / "first");
    dataDir.unset();

    const auto first = elizaos::eliza::getElizaDataPath();
    elizaos::eliza::resetPathCache();
    rootDir.set(base_ / "second");

    const auto second = elizaos::eliza::getElizaDataPath();
    EXPECT_NE(first, second);
    EXPECT_EQ(std::filesystem::path(second), (base_ / "second").lexically_normal());
}

TEST_F(ElizaPathsTest, IdempotentDirectoryCreation) {
    ScopedEnv rootDir("ELIZAOS_ROOT_DIR");
    ScopedEnv dataDir("ELIZAOS_DATA_DIR");
    rootDir.set(base_ / "idempotent");
    dataDir.unset();

    // Call multiple times - should not throw
    elizaos::eliza::getElizaDataPath();
    elizaos::eliza::getElizaConfigPath();
    elizaos::eliza::getElizaLogsPath();
    elizaos::eliza::getElizaCharactersPath();
    elizaos::eliza::getElizaKnowledgePath();
    elizaos::eliza::getElizaPluginsPath();
    elizaos::eliza::getElizaStatePath();

    // Call again - still works
    EXPECT_NO_THROW(elizaos::eliza::getElizaDataPath());
    EXPECT_NO_THROW(elizaos::eliza::getElizaConfigPath());
}
