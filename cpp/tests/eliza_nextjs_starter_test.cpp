// eliza_nextjs_starter_test.cpp - Behavioral tests for the Next.js generator.
#include <gtest/gtest.h>

#include "elizaos/eliza_nextjs_starter.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <process.h>
#define elizaos_getpid _getpid
#else
#include <unistd.h>
#define elizaos_getpid getpid
#endif

using namespace elizaos::nextjs_starter;
namespace fs = std::filesystem;

namespace {

class ScopedTempDirectory {
public:
    explicit ScopedTempDirectory(const std::string& name) {
        path_ = fs::temp_directory_path() /
                (name + "_" + std::to_string(elizaos_getpid()) + "_" +
                 std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::error_code ec;
        fs::create_directories(path_, ec);
        if (ec) throw std::runtime_error(ec.message());
    }

    ~ScopedTempDirectory() {
        std::error_code ec;
        fs::remove_all(path_, ec);
    }

    const fs::path& path() const { return path_; }

private:
    fs::path path_;
};

std::string readFile(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream content;
    content << input.rdbuf();
    return content.str();
}

void expectBaseProject(const fs::path& root) {
    const std::vector<fs::path> required = {
        "package.json", "tsconfig.json", "next.config.js", "README.md",
        "pages/_app.tsx", "pages/index.tsx", "pages/api/chat.ts",
        "components/ElizaChat.tsx", "lib/eliza.ts", "styles/globals.css",
    };
    for (const auto& relative : required) {
        EXPECT_TRUE(fs::is_regular_file(root / relative)) << relative.string();
    }
}

} // namespace

TEST(NextJSTemplateGenerator, WriteFileStaticCreatesParentsAndContent) {
    ScopedTempDirectory temp("elizaos_nextjs_write");
    const fs::path destination = temp.path() / "nested/file.txt";
    ASSERT_TRUE(NextJSTemplateGenerator::writeFileStatic(destination.string(), "content\n"));
    EXPECT_EQ(readFile(destination), "content\n");
    EXPECT_FALSE(NextJSTemplateGenerator::writeFileStatic("", "invalid"));
}

TEST(GenerateNextJSProject, RejectsInvalidArguments) {
    ScopedTempDirectory temp("elizaos_nextjs_invalid");
    EXPECT_FALSE(generateNextJSProject("", "demo"));
    EXPECT_FALSE(generateNextJSProject(temp.path().string(), ""));
    EXPECT_FALSE(generateNextJSProject(temp.path().string(), ".."));
}

TEST(GenerateNextJSProject, EmitsCompleteTypedProjectWithoutCppArtifacts) {
    ScopedTempDirectory temp("elizaos_nextjs_base");
    const fs::path project = temp.path() / "demo";
    ASSERT_TRUE(generateNextJSProject(project.string(), "demo-app"));
    expectBaseProject(project);

    const auto packageJson = readFile(project / "package.json");
    EXPECT_NE(packageJson.find("\"name\": \"demo-app\""), std::string::npos);
    EXPECT_NE(packageJson.find("\"typecheck\": \"tsc --noEmit\""), std::string::npos);

    const auto page = readFile(project / "pages/index.tsx");
    EXPECT_NE(page.find("export default function Home()"), std::string::npos);
    EXPECT_EQ(page.find("std::"), std::string::npos);

    const auto component = readFile(project / "components/ElizaChat.tsx");
    EXPECT_NE(component.find("messages.map"), std::string::npos);
    EXPECT_NE(component.find("async function submit"), std::string::npos);
    EXPECT_NE(component.find("aria-live"), std::string::npos);
    EXPECT_EQ(component.find("std::"), std::string::npos);

    const auto api = readFile(project / "pages/api/chat.ts");
    EXPECT_NE(api.find("ELIZAOS_API_URL"), std::string::npos);
    EXPECT_NE(api.find("request.method !== 'POST'"), std::string::npos);
    EXPECT_NE(api.find("status(502)"), std::string::npos);

    const auto library = readFile(project / "lib/eliza.ts");
    EXPECT_NE(library.find("Promise<string>"), std::string::npos);
    EXPECT_NE(library.find("Invalid ElizaOS response"), std::string::npos);
    EXPECT_FALSE(fs::exists(project / "hooks/useElizaSocket.ts"));
}

TEST(WebSocketIntegration, GeneratesTypedHookWithLifecycleAndSendGuard) {
    ScopedTempDirectory temp("elizaos_nextjs_socket_only");
    ASSERT_TRUE(WebSocketIntegration::generateWebSocketSupport(temp.path().string()));
    const fs::path hook = temp.path() / "hooks/useElizaSocket.ts";
    ASSERT_TRUE(fs::is_regular_file(hook));
    const auto content = readFile(hook);
    EXPECT_NE(content.find("new WebSocket(url)"), std::string::npos);
    EXPECT_NE(content.find("WebSocket.OPEN"), std::string::npos);
    EXPECT_NE(content.find("connection.close()"), std::string::npos);
    EXPECT_NE(content.find("NEXT_PUBLIC_ELIZAOS_WS_URL"), std::string::npos);
    EXPECT_EQ(content.find("std::"), std::string::npos);
}

TEST(GenerateNextJSProjectWithWebSocket, ImplementsDeclaredCombinedGenerator) {
    ScopedTempDirectory temp("elizaos_nextjs_socket");
    const fs::path project = temp.path() / "demo-ws";
    ASSERT_TRUE(generateNextJSProjectWithWebSocket(project.string(), "demo-ws"));
    expectBaseProject(project);
    EXPECT_TRUE(fs::is_regular_file(project / "hooks/useElizaSocket.ts"));
    EXPECT_NE(readFile(project / "README.md").find("NEXT_PUBLIC_ELIZAOS_WS_URL"),
              std::string::npos);
}

TEST(GenerateNextJSProject, RegenerationIsDeterministicAndRepairsChangedFiles) {
    ScopedTempDirectory temp("elizaos_nextjs_regenerate");
    const fs::path project = temp.path() / "demo";
    ASSERT_TRUE(generateNextJSProject(project.string(), "demo"));
    ASSERT_TRUE(NextJSTemplateGenerator::writeFileStatic(
        (project / "components/ElizaChat.tsx").string(), "corrupted"));
    ASSERT_TRUE(generateNextJSProject(project.string(), "demo"));
    EXPECT_NE(readFile(project / "components/ElizaChat.tsx").find("export function ElizaChat"),
              std::string::npos);
}
