// elizaos_github_io_test.cpp - Tests for static-site / docs generation.
#include <gtest/gtest.h>
#include "elizaos/elizaos_github_io.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <thread>

using namespace elizaos;
namespace fs = std::filesystem;

TEST(MarkdownProcessor, ConstructAndUse) {
    MarkdownProcessor p;
    SUCCEED();
}

TEST(DocumentationGenerator, ConstructAndUse) {
    GitHubPagesConfig cfg;
    DocumentationGenerator g(cfg);
    SUCCEED();
}

TEST(GitHubPagesDeployer, ConstructAndUse) {
    GitHubPagesConfig cfg;
    GitHubPagesDeployer d(cfg);
    SUCCEED();
}

TEST(ElizaOSGitHubIO, ConstructAndUse) {
    ElizaOSGitHubIO io;
    SUCCEED();
}

TEST(GitHubPagesConfig, BasicConstruction) {
    GitHubPagesConfig c;
    SUCCEED();
}

TEST(ElizaOSGitHubIO, InitializeValidatesGitAndCreatesWorkspace) {
    const fs::path root = fs::temp_directory_path() /
        ("github_io_" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    GitHubPagesConfig config;
    config.docs_dir = root / "docs";
    config.output_dir = root / "output";
    ElizaOSGitHubIO io(config);
    ASSERT_TRUE(io.initialize());
    EXPECT_TRUE(io.isInitialized());
    EXPECT_TRUE(fs::is_directory(config.docs_dir));
    EXPECT_TRUE(fs::is_directory(config.output_dir));
    EXPECT_NE(io.getDocumentationGenerator(), nullptr);
    EXPECT_NE(io.getDeployer(), nullptr);
    EXPECT_NE(io.getMarkdownProcessor(), nullptr);
    fs::remove_all(root);
}

#ifndef _WIN32
TEST(ElizaOSGitHubIO, InitializeRejectsMissingGitExecutable) {
    const fs::path root = fs::temp_directory_path() /
        ("github_io_no_git_" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    GitHubPagesConfig config;
    config.docs_dir = root / "docs";
    config.output_dir = root / "output";

    const char* current = std::getenv("PATH");
    const std::string original = current ? current : "";
    ASSERT_EQ(::setenv("PATH", "/nonexistent", 1), 0);
    ElizaOSGitHubIO io(config);
    EXPECT_FALSE(io.initialize());
    EXPECT_FALSE(io.isInitialized());
    ASSERT_EQ(::setenv("PATH", original.c_str(), 1), 0);
    fs::remove_all(root);
}
#endif

TEST(ElizaOSGitHubIO, WatchesRealDirectoryAndShutsDownCleanly) {
    const fs::path root = fs::temp_directory_path() /
        ("github_io_watch_" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    GitHubPagesConfig config;
    config.docs_dir = root / "docs";
    config.output_dir = root / "output";
    const fs::path watched = root / "watched";
    fs::create_directories(watched);

    {
        ElizaOSGitHubIO io(config);
        EXPECT_FALSE(io.watchForChanges(watched));
        ASSERT_TRUE(io.initialize());
        EXPECT_FALSE(io.watchForChanges(root / "missing"));
        EXPECT_FALSE(io.enableAutoDeployment(""));
        EXPECT_TRUE(io.enableAutoDeployment("main"));
        EXPECT_TRUE(io.disableAutoDeployment());
        ASSERT_TRUE(io.watchForChanges(watched));
        EXPECT_TRUE(io.isWatching());
        {
            std::ofstream output(watched / "change.md");
            output << "# change\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        EXPECT_TRUE(io.isWatching());
    }

    fs::remove_all(root);
}
