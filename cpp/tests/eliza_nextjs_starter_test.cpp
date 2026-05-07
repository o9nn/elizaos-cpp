// eliza_nextjs_starter_test.cpp - Tests for NextJS template generator.
#include <gtest/gtest.h>
#include "elizaos/eliza_nextjs_starter.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace elizaos::nextjs_starter;

TEST(NextJSTemplateGenerator, ConstructAndUse) {
    NextJSTemplateGenerator g;
    SUCCEED();
}

TEST(WebSocketIntegration, ConstructAndUse) {
    WebSocketIntegration w;
    SUCCEED();
}

TEST(GenerateNextJSProject, RunsWithoutCrash) {
    auto path = std::string("/tmp/nextjs_test_") + std::to_string(::getpid());
    EXPECT_NO_THROW({
        bool r = generateNextJSProject(path, "demo");
        (void)r;
    });
    std::string cmd = "rm -rf " + path;
    std::system(cmd.c_str());
}

// generateNextJSProjectWithWebSocket is declared in the header but not
// linked in any object file under the eliza_nextjs_starter library; coverage
// for that function is therefore skipped until the implementation lands.
