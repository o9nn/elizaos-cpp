#include <gtest/gtest.h>
#include "elizaos/eliza_nextjs_starter.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

using namespace elizaos::nextjs_starter;
namespace fs = std::filesystem;

namespace {

fs::path uniquePath(const std::string& prefix) {
    return fs::temp_directory_path() /
        (prefix + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
}

std::string readText(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

} // namespace

TEST(NextJSTemplateGenerator, PublicTypesConstruct) {
    NextJSTemplateGenerator generator;
    WebSocketIntegration websocket;
    EXPECT_NE(&generator, nullptr);
    EXPECT_NE(&websocket, nullptr);
}

TEST(NextJSTemplateGenerator, RejectsInvalidProjectInputs) {
    EXPECT_FALSE(generateNextJSProject("", "demo"));
    EXPECT_FALSE(generateNextJSProject(uniquePath("nextjs_empty_name_").string(), ""));
    EXPECT_FALSE(generateNextJSProjectWithWebSocket("", "demo"));
}

TEST(NextJSTemplateGenerator, GeneratesCompleteTypedHttpProject) {
    const fs::path root = uniquePath("nextjs_http_");
    fs::remove_all(root);
    ASSERT_TRUE(generateNextJSProject(root.string(), "demo-eliza"));

    const std::vector<fs::path> required = {
        "package.json", "tsconfig.json", "next.config.js", ".env.example", "README.md",
        "pages/index.tsx", "pages/_app.tsx", "pages/api/chat.ts",
        "components/ElizaChat.tsx", "lib/elizaApi.ts", "styles/globals.css"};
    for (const auto& relative : required) {
        EXPECT_TRUE(fs::is_regular_file(root / relative)) << relative;
    }

    const std::string page = readText(root / "pages/index.tsx");
    const std::string component = readText(root / "components/ElizaChat.tsx");
    const std::string route = readText(root / "pages/api/chat.ts");
    const std::string package = readText(root / "package.json");
    EXPECT_NE(page.find("export default function Home"), std::string::npos);
    EXPECT_EQ(page.find("std::"), std::string::npos);
    EXPECT_NE(component.find("sendElizaMessage"), std::string::npos);
    EXPECT_EQ(component.find("TODO"), std::string::npos);
    EXPECT_NE(route.find("ELIZAOS_API_URL"), std::string::npos);
    EXPECT_NE(route.find("AbortSignal.timeout(30_000)"), std::string::npos);
    EXPECT_NE(package.find("\"typecheck\": \"tsc --noEmit\""), std::string::npos);
    EXPECT_EQ(package.find("@elizaos/client"), std::string::npos);
    fs::remove_all(root);
}

TEST(NextJSTemplateGenerator, PublicWriterCreatesParentsAndOverwrites) {
    const fs::path root = uniquePath("nextjs_writer_");
    const fs::path file = root / "nested" / "artifact.txt";
    ASSERT_TRUE(NextJSTemplateGenerator::writeFileStatic(file.string(), "first"));
    EXPECT_EQ(readText(file), "first");
    ASSERT_TRUE(NextJSTemplateGenerator::writeFileStatic(file.string(), "second"));
    EXPECT_EQ(readText(file), "second");
    EXPECT_FALSE(fs::exists(file.string() + ".tmp"));
    fs::remove_all(root);
}

TEST(WebSocketIntegration, GeneratesCompleteReconnectClient) {
    const fs::path root = uniquePath("nextjs_websocket_");
    fs::remove_all(root);
    ASSERT_TRUE(generateNextJSProjectWithWebSocket(root.string(), "demo-eliza-ws"));
    const fs::path socketFile = root / "lib/elizaSocket.ts";
    ASSERT_TRUE(fs::is_regular_file(socketFile));
    const std::string socket = readText(socketFile);
    EXPECT_NE(socket.find("export class ElizaSocket"), std::string::npos);
    EXPECT_NE(socket.find("scheduleReconnect"), std::string::npos);
    EXPECT_NE(socket.find("JSON.parse"), std::string::npos);
    EXPECT_EQ(socket.find("TODO"), std::string::npos);
    fs::remove_all(root);
}
