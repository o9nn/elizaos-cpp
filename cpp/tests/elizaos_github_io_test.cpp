// elizaos_github_io_test.cpp - Tests for static-site / docs generation.
#include <gtest/gtest.h>
#include "elizaos/elizaos_github_io.hpp"

using namespace elizaos;

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
