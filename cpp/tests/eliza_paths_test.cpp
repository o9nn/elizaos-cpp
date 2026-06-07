#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <string>
#include <unistd.h>

namespace elizaos {
namespace eliza {
std::string getElizaDataPath();
std::string getElizaConfigPath();
std::string getElizaLogsPath();
} // namespace eliza
} // namespace elizaos

namespace {

class ScopedEnv {
public:
    explicit ScopedEnv(const char* name)
        : name_(name) {
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

private:
    const char* name_;
    bool had_value_{false};
    std::string old_value_;
};

} // namespace

TEST(ElizaPathsTest, UsesExplicitDataDirectoryAndCreatesSubdirectories) {
    ScopedEnv data_dir("ELIZAOS_DATA_DIR");
    const auto base = std::filesystem::temp_directory_path() / "elizaos_cpp_paths_test" / std::to_string(::getpid());
    std::filesystem::remove_all(base);
    data_dir.set(base);

    const auto data = std::filesystem::path(elizaos::eliza::getElizaDataPath());
    const auto config = std::filesystem::path(elizaos::eliza::getElizaConfigPath());
    const auto logs = std::filesystem::path(elizaos::eliza::getElizaLogsPath());

    EXPECT_EQ(data, base.lexically_normal());
    EXPECT_EQ(config, (base / "config").lexically_normal());
    EXPECT_EQ(logs, (base / "logs").lexically_normal());
    EXPECT_TRUE(std::filesystem::is_directory(data));
    EXPECT_TRUE(std::filesystem::is_directory(config));
    EXPECT_TRUE(std::filesystem::is_directory(logs));

    std::filesystem::remove_all(base);
}

TEST(ElizaPathsTest, FallsBackToXdgStateHomeWhenExplicitDirectoryIsUnset) {
    ScopedEnv data_dir("ELIZAOS_DATA_DIR");
    ScopedEnv xdg_state_home("XDG_STATE_HOME");
    unsetenv("ELIZAOS_DATA_DIR");

    const auto state_home = std::filesystem::temp_directory_path() / "elizaos_cpp_xdg_test" / std::to_string(::getpid());
    std::filesystem::remove_all(state_home);
    xdg_state_home.set(state_home);

    const auto data = std::filesystem::path(elizaos::eliza::getElizaDataPath());

    EXPECT_EQ(data, (state_home / "elizaos").lexically_normal());
    EXPECT_TRUE(std::filesystem::is_directory(data));

    std::filesystem::remove_all(state_home);
}
