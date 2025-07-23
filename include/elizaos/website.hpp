#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <functional>

namespace elizaos {

/**
 * Website Module - Main website functionality and content management
 * Provides static site generation and content management capabilities
 */

class ContentManager;
class TemplateEngine;
class StaticSiteGenerator;

/**
 * Represents a website page with metadata and content
 */
struct WebPage {
    std::string id;
    std::string title;
    std::string content;
    std::string template_name;
    std::unordered_map<std::string, std::string> metadata;
    std::filesystem::path source_path;
    std::filesystem::path output_path;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    
    WebPage(const std::string& page_id, const std::string& page_title, const std::string& page_content)
        : id(page_id), title(page_title), content(page_content), 
          created_at(std::chrono::system_clock::now()),
          updated_at(std::chrono::system_clock::now()) {}
};

/**
 * Configuration for website generation
 */
struct WebsiteConfig {
    std::filesystem::path source_dir;
    std::filesystem::path output_dir;
    std::filesystem::path templates_dir;
    std::filesystem::path assets_dir;
    std::string base_url;
    std::string site_title;
    std::string site_description;
    std::unordered_map<std::string, std::string> global_vars;
    
    WebsiteConfig() 
        : source_dir("./src"), 
          output_dir("./dist"),
          templates_dir("./templates"),
          assets_dir("./assets"),
          base_url("https://elizaos.github.io"),
          site_title("ElizaOS"),
          site_description("C++ implementation of ElizaOS agent framework") {}
};

/**
 * Simple template engine for static site generation
 */
class TemplateEngine {
public:
    TemplateEngine();
    ~TemplateEngine();
    
    // Template management
    bool loadTemplate(const std::string& name, const std::filesystem::path& template_path);
    bool hasTemplate(const std::string& name) const;
    
    // Template rendering
    std::string render(const std::string& template_name, 
                      const std::unordered_map<std::string, std::string>& variables) const;
    std::string renderString(const std::string& template_content,
                           const std::unordered_map<std::string, std::string>& variables) const;
    
    // Variable substitution
    void setGlobalVariable(const std::string& key, const std::string& value);
    std::string getGlobalVariable(const std::string& key) const;
    
private:
    std::unordered_map<std::string, std::string> templates_;
    std::unordered_map<std::string, std::string> global_variables_;
    
    std::string substituteVariables(const std::string& content, 
                                   const std::unordered_map<std::string, std::string>& variables) const;
};

/**
 * Content management system for websites
 */
class ContentManager {
public:
    ContentManager(const WebsiteConfig& config);
    ~ContentManager();
    
    // Page management
    bool addPage(const WebPage& page);
    bool removePage(const std::string& page_id);
    std::shared_ptr<WebPage> getPage(const std::string& page_id) const;
    std::vector<std::shared_ptr<WebPage>> getAllPages() const;
    std::vector<std::shared_ptr<WebPage>> getPagesByTemplate(const std::string& template_name) const;
    
    // Content operations
    bool loadPagesFromDirectory(const std::filesystem::path& directory);
    bool savePage(const WebPage& page, const std::filesystem::path& output_path) const;
    
    // Metadata operations
    std::unordered_map<std::string, std::string> parsePageMetadata(const std::string& content) const;
    std::string stripMetadata(const std::string& content) const;
    
    // Configuration
    const WebsiteConfig& getConfig() const { return config_; }
    void updateConfig(const WebsiteConfig& config) { config_ = config; }
    
private:
    WebsiteConfig config_;
    std::unordered_map<std::string, std::shared_ptr<WebPage>> pages_;
    
    std::string generatePageId(const std::filesystem::path& file_path) const;
    bool isMarkdownFile(const std::filesystem::path& file_path) const;
    bool isHtmlFile(const std::filesystem::path& file_path) const;
};

/**
 * Static site generator
 */
class StaticSiteGenerator {
public:
    StaticSiteGenerator(const WebsiteConfig& config);
    ~StaticSiteGenerator();
    
    // Generation operations
    bool generateSite();
    bool generatePage(const std::string& page_id);
    bool copyAssets();
    bool cleanOutputDirectory();
    
    // Component integration
    void setContentManager(std::shared_ptr<ContentManager> content_manager);
    void setTemplateEngine(std::shared_ptr<TemplateEngine> template_engine);
    
    // Generation status
    struct GenerationStats {
        size_t pages_generated = 0;
        size_t assets_copied = 0;
        size_t errors = 0;
        std::chrono::milliseconds generation_time{0};
        std::vector<std::string> error_messages;
    };
    
    const GenerationStats& getLastGenerationStats() const { return last_stats_; }
    
    // Configuration
    const WebsiteConfig& getConfig() const { return config_; }
    void updateConfig(const WebsiteConfig& config) { config_ = config; }
    
private:
    WebsiteConfig config_;
    std::shared_ptr<ContentManager> content_manager_;
    std::shared_ptr<TemplateEngine> template_engine_;
    GenerationStats last_stats_;
    
    bool ensureOutputDirectory();
    bool generatePageFile(const WebPage& page);
    bool copyFile(const std::filesystem::path& source, const std::filesystem::path& destination);
    bool copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination);
    std::string markdownToHtml(const std::string& markdown) const;
};

/**
 * Website module main interface
 */
class Website {
public:
    Website(const WebsiteConfig& config = WebsiteConfig{});
    ~Website();
    
    // Initialization
    bool initialize();
    bool isInitialized() const { return initialized_; }
    
    // Site generation
    bool generateSite();
    bool serveDevelopmentSite(int port = 8080);
    bool watchForChanges(bool enable = true);
    
    // Content management
    std::shared_ptr<ContentManager> getContentManager() const { return content_manager_; }
    std::shared_ptr<TemplateEngine> getTemplateEngine() const { return template_engine_; }
    std::shared_ptr<StaticSiteGenerator> getGenerator() const { return generator_; }
    
    // Configuration
    const WebsiteConfig& getConfig() const { return config_; }
    bool updateConfig(const WebsiteConfig& config);
    
    // Statistics
    StaticSiteGenerator::GenerationStats getGenerationStats() const;
    
private:
    WebsiteConfig config_;
    std::shared_ptr<ContentManager> content_manager_;
    std::shared_ptr<TemplateEngine> template_engine_;
    std::shared_ptr<StaticSiteGenerator> generator_;
    bool initialized_ = false;
    bool watching_ = false;
    
    bool setupDirectories();
    bool loadDefaultTemplates();
};

} // namespace elizaos