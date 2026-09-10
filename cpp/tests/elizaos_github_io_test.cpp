// elizaos_github_io_test.cpp - Behavioral tests for static-site generation and deployment.
#include <gtest/gtest.h>
#include "elizaos/elizaos_github_io.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

using namespace elizaos;
namespace fs = std::filesystem;

namespace {

class TempDirectory {
public:
    explicit TempDirectory(const std::string& label) {
        const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = fs::temp_directory_path() /
            ("elizaos-github-io-" + label + "-" + std::to_string(nonce) + "-" +
             std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())));
        fs::create_directories(path_);
    }

    ~TempDirectory() {
        std::error_code error;
        fs::remove_all(path_, error);
    }

    const fs::path& path() const noexcept { return path_; }

private:
    fs::path path_;
};

void writeFile(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream output(path, std::ios::binary);
    ASSERT_TRUE(output.is_open());
    output << content;
    ASSERT_TRUE(output.good());
}

GitHubPagesConfig localConfig(const fs::path& output) {
    GitHubPagesConfig config("owner", "repository");
    config.branch = "gh-pages";
    config.access_token = "test-token-never-used-for-local-git";
    config.output_dir = output;
    return config;
}

}  // namespace

TEST(MarkdownProcessor, RendersEscapedCodeHeadingsLinksAndTasks) {
    MarkdownProcessor processor;
    const std::string markdown =
        "# Heading\n"
        "[site](https://example.com)\n"
        "- [x] complete\n"
        "```cpp\nif (a < b) return;\n```\n";

    const auto html = processor.markdownToHtml(markdown);
    EXPECT_NE(html.find("<h1>Heading</h1>"), std::string::npos);
    EXPECT_NE(html.find("href=\"https://example.com\""), std::string::npos);
    EXPECT_NE(html.find("checked"), std::string::npos);
    EXPECT_NE(html.find("if (a &lt; b) return;"), std::string::npos);
}

TEST(DocumentationGenerator, AddsFiltersAndRemovesPages) {
    TempDirectory temp("docs");
    DocumentationGenerator generator(localConfig(temp.path()));

    DocumentationPage page("Autonomy", "content");
    page.output_path = "autonomy.html";
    page.tags = {"agent", "autonomy"};
    EXPECT_TRUE(generator.addPage(page));
    EXPECT_EQ(generator.getAllPages().size(), 1u);
    ASSERT_EQ(generator.getPagesByTag("autonomy").size(), 1u);
    EXPECT_TRUE(generator.removePage("autonomy.html"));
    EXPECT_TRUE(generator.getAllPages().empty());
    EXPECT_FALSE(generator.removePage("missing.html"));
}

TEST(GitHubPagesDeployer, InitializesARealRepositoryAndCommitsWithoutShellExpansion) {
    TempDirectory temp("local-git");
    const auto repo = temp.path() / "repository with spaces";
    const auto sentinel = temp.path() / "must-not-exist";

    GitHubPagesDeployer deployer(localConfig(temp.path()));
    ASSERT_TRUE(deployer.initializeGitRepo(repo));
    EXPECT_TRUE(fs::is_directory(repo / ".git"));

    writeFile(repo / "index.html", "<h1>Eliza</h1>\n");
    const std::string message = "publish; touch " + sentinel.string();
    ASSERT_TRUE(deployer.commitChanges(message));
    EXPECT_FALSE(fs::exists(sentinel));

    const auto& status = deployer.getLastDeploymentStatus();
    EXPECT_GE(status.commit_sha.size(), 40u);
    EXPECT_TRUE(status.errors.empty());
    EXPECT_TRUE(deployer.commitChanges("no-op commit is accepted"));
    EXPECT_FALSE(status.warnings.empty());
}

TEST(GitHubPagesDeployer, CopiesACompleteSnapshotAndRemovesStaleFiles) {
    TempDirectory temp("copy");
    const auto source = temp.path() / "source";
    const auto repository = temp.path() / "repository";
    writeFile(source / "nested" / "new.txt", "new-data");

    GitHubPagesDeployer deployer(localConfig(source));
    ASSERT_TRUE(deployer.initializeGitRepo(repository));
    writeFile(repository / "stale.txt", "obsolete");

    ASSERT_TRUE(deployer.copyFilesToRepo(source, repository));
    EXPECT_FALSE(fs::exists(repository / "stale.txt"));
    EXPECT_TRUE(fs::is_directory(repository / ".git"));
    EXPECT_TRUE(fs::is_regular_file(repository / "nested" / "new.txt"));

    std::ifstream copied(repository / "nested" / "new.txt", std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(copied)), {});
    EXPECT_EQ(content, "new-data");
    EXPECT_FALSE(deployer.copyFilesToRepo(repository, repository));
}

#ifndef _WIN32
TEST(GitHubPagesDeployer, RejectsSymbolicLinksInDeploymentSnapshot) {
    TempDirectory temp("symlink");
    const auto source = temp.path() / "source";
    const auto repository = temp.path() / "repository";
    writeFile(source / "target.txt", "target");
    fs::create_symlink(source / "target.txt", source / "alias.txt");

    GitHubPagesDeployer deployer(localConfig(source));
    ASSERT_TRUE(deployer.initializeGitRepo(repository));
    EXPECT_FALSE(deployer.copyFilesToRepo(source, repository));
    EXPECT_FALSE(deployer.getLastDeploymentStatus().errors.empty());
}
#endif

TEST(GitHubPagesDeployer, RejectsInvalidRefsAndTruthfullyReportsPushFailure) {
    TempDirectory temp("invalid-ref");
    const auto repository = temp.path() / "repository";
    auto config = localConfig(temp.path());
    GitHubPagesDeployer deployer(config);
    ASSERT_TRUE(deployer.initializeGitRepo(repository));

    const auto sentinel = temp.path() / "branch-injection";
    EXPECT_FALSE(deployer.createBranch("bad;touch " + sentinel.string()));
    EXPECT_FALSE(fs::exists(sentinel));
    EXPECT_FALSE(deployer.switchToBranch("missing-branch"));
    EXPECT_FALSE(deployer.pushToGitHub());
    EXPECT_FALSE(deployer.getLastDeploymentStatus().errors.empty());
}

TEST(GitHubPagesDeployer, RejectsMissingSourceAndInvalidConfiguration) {
    TempDirectory temp("invalid-config");
    auto config = localConfig(temp.path() / "missing-output");
    config.repository_owner.clear();
    GitHubPagesDeployer deployer(config);

    EXPECT_FALSE(deployer.deploy());
    EXPECT_FALSE(deployer.deployDirectory(temp.path() / "missing-source"));
}

TEST(ElizaOSGitHubIO, InitializationRequiresAValidWorkspace) {
    TempDirectory temp("facade");
    auto config = localConfig(temp.path() / "generated");
    config.docs_dir = temp.path() / "docs";
    fs::create_directories(config.docs_dir);

    ElizaOSGitHubIO io(config);
    EXPECT_TRUE(io.initialize());
    EXPECT_TRUE(io.isInitialized());
    EXPECT_NE(io.getDocumentationGenerator(), nullptr);
    EXPECT_NE(io.getDeployer(), nullptr);
    EXPECT_NE(io.getMarkdownProcessor(), nullptr);
}
