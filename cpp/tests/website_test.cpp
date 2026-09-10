// website_test.cpp - Focused tests for Website generation and validation.
#include <gtest/gtest.h>

#include "elizaos/website.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace elizaos {
namespace {

class TemporaryDirectory {
public:
    TemporaryDirectory() {
        const auto suffix = std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count());
        path_ = std::filesystem::temp_directory_path() / ("elizaos-website-unit-" + suffix);
        std::filesystem::create_directories(path_);
    }

    ~TemporaryDirectory() {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

WebsiteConfig configFor(const std::filesystem::path& root) {
    WebsiteConfig config;
    config.source_dir = root / "source";
    config.output_dir = root / "output";
    config.templates_dir = root / "templates";
    config.assets_dir = root / "assets";
    config.site_title = "Focused Test Site";
    return config;
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
}

TEST(TemplateEngineTest, SubstitutesLocalAndGlobalVariables) {
    TemplateEngine engine;
    engine.setGlobalVariable("site", "ElizaOS");

    EXPECT_EQ(engine.renderString("{{site}}/{{page}}/{{missing}}", {{"page", "docs"}}),
              "ElizaOS/docs/{{missing}}");
    EXPECT_TRUE(engine.render("missing", {}).empty());
    EXPECT_FALSE(engine.loadTemplate("", {}));
}

TEST(ContentManagerTest, ParsesAndStripsUnixAndWindowsFrontMatter) {
    ContentManager manager(WebsiteConfig{});
    const std::string unix_content = "---\ntitle: Hello\ntemplate: markdown\n---\n# Body";
    const std::string windows_content =
        "---\r\ntitle: Windows\r\ntemplate: html\r\n---\r\nBody";

    const auto unix_metadata = manager.parsePageMetadata(unix_content);
    ASSERT_EQ(unix_metadata.at("title"), "Hello");
    EXPECT_EQ(manager.stripMetadata(unix_content), "# Body");

    const auto windows_metadata = manager.parsePageMetadata(windows_content);
    ASSERT_EQ(windows_metadata.at("title"), "Windows");
    EXPECT_EQ(manager.stripMetadata(windows_content), "Body");
    EXPECT_FALSE(manager.addPage(WebPage("../escape", "Escape", "x")));
}

TEST(WebsiteGenerationTest, PreservesStaticGenerationAndCountsAssets) {
    TemporaryDirectory temporary;
    const WebsiteConfig config = configFor(temporary.path());
    std::filesystem::create_directories(config.source_dir);
    std::filesystem::create_directories(config.assets_dir / "css");
    {
        std::ofstream source(config.source_dir / "home.md");
        source << "---\ntitle: Welcome\n---\n# Hello\n\nGenerated body";
    }
    {
        std::ofstream asset(config.assets_dir / "css" / "site.css");
        asset << "body { color: black; }";
    }

    Website website(config);
    ASSERT_TRUE(website.initialize());
    ASSERT_TRUE(website.generateSite());

    const auto generated = readFile(config.output_dir / "home.html");
    EXPECT_NE(generated.find("Welcome - Focused Test Site"), std::string::npos);
    EXPECT_NE(generated.find("<h1>Hello</h1>"), std::string::npos);
    EXPECT_EQ(readFile(config.output_dir / "assets" / "css" / "site.css"),
              "body { color: black; }");

    const auto stats = website.getGenerationStats();
    EXPECT_EQ(stats.pages_generated, 1U);
    EXPECT_EQ(stats.assets_copied, 1U);
    EXPECT_EQ(stats.errors, 0U);
}

TEST(WebsiteGenerationTest, RejectsPageOutputOutsideConfiguredRoot) {
    TemporaryDirectory temporary;
    const WebsiteConfig config = configFor(temporary.path());
    Website website(config);
    ASSERT_TRUE(website.initialize());

    WebPage page("escape", "Escape", "content");
    page.template_name = "html";
    page.output_path = temporary.path() / "outside.html";
    ASSERT_TRUE(website.getContentManager()->addPage(page));
    EXPECT_FALSE(website.getGenerator()->generatePage("escape"));
    EXPECT_FALSE(std::filesystem::exists(page.output_path));
}

TEST(WebsiteLifecycleTest, IsIdempotentAndUnsupportedWatchingFailsTruthfully) {
    TemporaryDirectory temporary;
    Website website(configFor(temporary.path()));

    EXPECT_FALSE(website.startDevelopmentServer(0));
    EXPECT_FALSE(website.getLastDevelopmentServerError().empty());
    EXPECT_TRUE(website.initialize());
    EXPECT_TRUE(website.initialize());
    EXPECT_FALSE(website.watchForChanges(true));
    EXPECT_TRUE(website.watchForChanges(false));
}

TEST(WebsiteValidationTest, RejectsUnsafeDirectoryConfiguration) {
    WebsiteConfig config;
    config.output_dir.clear();
    Website website(config);
    EXPECT_FALSE(website.initialize());

    TemporaryDirectory temporary;
    Website valid(configFor(temporary.path()));
    ASSERT_TRUE(valid.initialize());
    WebsiteConfig unsafe = valid.getConfig();
    unsafe.output_dir.clear();
    EXPECT_FALSE(valid.updateConfig(unsafe));
}

} // namespace
} // namespace elizaos
