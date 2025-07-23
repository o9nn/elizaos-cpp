#include "elizaos/website.hpp"
#include "elizaos/agentlogger.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace elizaos {

// Global logger instance for the module
static AgentLogger g_website_logger;

// TemplateEngine implementation
TemplateEngine::TemplateEngine() = default;
TemplateEngine::~TemplateEngine() = default;

bool TemplateEngine::loadTemplate(const std::string& name, const std::filesystem::path& template_path) {
    try {
        std::ifstream file(template_path);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        templates_[name] = buffer.str();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool TemplateEngine::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

std::string TemplateEngine::render(const std::string& template_name, 
                                  const std::unordered_map<std::string, std::string>& variables) const {
    auto it = templates_.find(template_name);
    if (it == templates_.end()) {
        return "";
    }
    
    return substituteVariables(it->second, variables);
}

std::string TemplateEngine::renderString(const std::string& template_content,
                                       const std::unordered_map<std::string, std::string>& variables) const {
    return substituteVariables(template_content, variables);
}

void TemplateEngine::setGlobalVariable(const std::string& key, const std::string& value) {
    global_variables_[key] = value;
}

std::string TemplateEngine::getGlobalVariable(const std::string& key) const {
    auto it = global_variables_.find(key);
    return it != global_variables_.end() ? it->second : "";
}

std::string TemplateEngine::substituteVariables(const std::string& content, 
                                               const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = content;
    
    // Merge global variables with local variables (local takes precedence)
    auto all_vars = global_variables_;
    for (const auto& var : variables) {
        all_vars[var.first] = var.second;
    }
    
    // Simple template substitution using {{variable}} syntax
    std::regex var_regex(R"(\{\{(\w+)\}\})");
    std::smatch matches;
    
    std::string::const_iterator start = result.cbegin();
    std::stringstream output;
    
    while (std::regex_search(start, result.cend(), matches, var_regex)) {
        output << std::string(start, matches[0].first);
        
        std::string var_name = matches[1].str();
        auto var_it = all_vars.find(var_name);
        if (var_it != all_vars.end()) {
            output << var_it->second;
        } else {
            output << matches[0].str(); // Keep original if variable not found
        }
        
        start = matches[0].second;
    }
    output << std::string(start, result.cend());
    
    return output.str();
}

// ContentManager implementation
ContentManager::ContentManager(const WebsiteConfig& config) : config_(config) {}
ContentManager::~ContentManager() = default;

bool ContentManager::addPage(const WebPage& page) {
    pages_[page.id] = std::make_shared<WebPage>(page);
    return true;
}

bool ContentManager::removePage(const std::string& page_id) {
    return pages_.erase(page_id) > 0;
}

std::shared_ptr<WebPage> ContentManager::getPage(const std::string& page_id) const {
    auto it = pages_.find(page_id);
    return it != pages_.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<WebPage>> ContentManager::getAllPages() const {
    std::vector<std::shared_ptr<WebPage>> result;
    for (const auto& pair : pages_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<WebPage>> ContentManager::getPagesByTemplate(const std::string& template_name) const {
    std::vector<std::shared_ptr<WebPage>> result;
    for (const auto& pair : pages_) {
        if (pair.second->template_name == template_name) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool ContentManager::loadPagesFromDirectory(const std::filesystem::path& directory) {
    try {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return false;
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && (isMarkdownFile(entry.path()) || isHtmlFile(entry.path()))) {
                std::ifstream file(entry.path());
                if (!file.is_open()) continue;
                
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
                
                std::string page_id = generatePageId(entry.path());
                auto metadata = parsePageMetadata(content);
                std::string clean_content = stripMetadata(content);
                
                std::string title = metadata.count("title") ? metadata["title"] : entry.path().stem().string();
                
                WebPage page(page_id, title, clean_content);
                page.source_path = entry.path();
                page.metadata = metadata;
                
                // Set template name from metadata or file extension
                if (metadata.count("template")) {
                    page.template_name = metadata["template"];
                } else if (isMarkdownFile(entry.path())) {
                    page.template_name = "markdown";
                } else {
                    page.template_name = "html";
                }
                
                addPage(page);
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ContentManager::savePage(const WebPage& page, const std::filesystem::path& output_path) const {
    try {
        std::filesystem::create_directories(output_path.parent_path());
        
        std::ofstream file(output_path);
        if (!file.is_open()) {
            return false;
        }
        
        file << page.content;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::unordered_map<std::string, std::string> ContentManager::parsePageMetadata(const std::string& content) const {
    std::unordered_map<std::string, std::string> metadata;
    
    // Simple front matter parsing (YAML-like between --- lines)
    if (content.substr(0, 3) != "---") {
        return metadata;
    }
    
    size_t end_pos = content.find("\n---\n", 3);
    if (end_pos == std::string::npos) {
        return metadata;
    }
    
    std::string front_matter = content.substr(3, end_pos - 3);
    std::istringstream stream(front_matter);
    std::string line;
    
    while (std::getline(stream, line)) {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            metadata[key] = value;
        }
    }
    
    return metadata;
}

std::string ContentManager::stripMetadata(const std::string& content) const {
    if (content.substr(0, 3) != "---") {
        return content;
    }
    
    size_t end_pos = content.find("\n---\n", 3);
    if (end_pos == std::string::npos) {
        return content;
    }
    
    return content.substr(end_pos + 5); // Skip the closing ---\n
}

std::string ContentManager::generatePageId(const std::filesystem::path& file_path) const {
    std::string id = file_path.stem().string();
    std::replace(id.begin(), id.end(), ' ', '_');
    std::transform(id.begin(), id.end(), id.begin(), ::tolower);
    return id;
}

bool ContentManager::isMarkdownFile(const std::filesystem::path& file_path) const {
    std::string ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".md" || ext == ".markdown";
}

bool ContentManager::isHtmlFile(const std::filesystem::path& file_path) const {
    std::string ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".html" || ext == ".htm";
}

// StaticSiteGenerator implementation
StaticSiteGenerator::StaticSiteGenerator(const WebsiteConfig& config) : config_(config) {}
StaticSiteGenerator::~StaticSiteGenerator() = default;

bool StaticSiteGenerator::generateSite() {
    auto start_time = std::chrono::steady_clock::now();
    last_stats_ = GenerationStats{};
    
    if (!ensureOutputDirectory()) {
        last_stats_.errors++;
        last_stats_.error_messages.push_back("Failed to create output directory");
        return false;
    }
    
    if (!content_manager_ || !template_engine_) {
        last_stats_.errors++;
        last_stats_.error_messages.push_back("Content manager or template engine not set");
        return false;
    }
    
    // Generate all pages
    auto pages = content_manager_->getAllPages();
    for (const auto& page : pages) {
        if (generatePageFile(*page)) {
            last_stats_.pages_generated++;
        } else {
            last_stats_.errors++;
            last_stats_.error_messages.push_back("Failed to generate page: " + page->id);
        }
    }
    
    // Copy assets
    if (copyAssets()) {
        // Asset copying count would be tracked in copyAssets implementation
    } else {
        last_stats_.errors++;
        last_stats_.error_messages.push_back("Failed to copy assets");
    }
    
    auto end_time = std::chrono::steady_clock::now();
    last_stats_.generation_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    return last_stats_.errors == 0;
}

bool StaticSiteGenerator::generatePage(const std::string& page_id) {
    if (!content_manager_) {
        return false;
    }
    
    auto page = content_manager_->getPage(page_id);
    if (!page) {
        return false;
    }
    
    return generatePageFile(*page);
}

bool StaticSiteGenerator::copyAssets() {
    try {
        if (!std::filesystem::exists(config_.assets_dir)) {
            return true; // No assets to copy is not an error
        }
        
        auto target_assets_dir = config_.output_dir / "assets";
        return copyDirectory(config_.assets_dir, target_assets_dir);
    } catch (const std::exception&) {
        return false;
    }
}

bool StaticSiteGenerator::cleanOutputDirectory() {
    try {
        if (std::filesystem::exists(config_.output_dir)) {
            std::filesystem::remove_all(config_.output_dir);
        }
        return std::filesystem::create_directories(config_.output_dir);
    } catch (const std::exception&) {
        return false;
    }
}

void StaticSiteGenerator::setContentManager(std::shared_ptr<ContentManager> content_manager) {
    content_manager_ = content_manager;
}

void StaticSiteGenerator::setTemplateEngine(std::shared_ptr<TemplateEngine> template_engine) {
    template_engine_ = template_engine;
}

bool StaticSiteGenerator::ensureOutputDirectory() {
    try {
        if (!std::filesystem::exists(config_.output_dir)) {
            return std::filesystem::create_directories(config_.output_dir);
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool StaticSiteGenerator::generatePageFile(const WebPage& page) {
    if (!template_engine_ || !template_engine_->hasTemplate(page.template_name)) {
        return false;
    }
    
    // Prepare template variables
    std::unordered_map<std::string, std::string> vars;
    vars["title"] = page.title;
    vars["content"] = page.template_name == "markdown" ? markdownToHtml(page.content) : page.content;
    
    // Add metadata as variables
    for (const auto& meta : page.metadata) {
        vars[meta.first] = meta.second;
    }
    
    // Add global config variables
    vars["site_title"] = config_.site_title;
    vars["site_description"] = config_.site_description;
    vars["base_url"] = config_.base_url;
    
    std::string rendered = template_engine_->render(page.template_name, vars);
    if (rendered.empty()) {
        return false;
    }
    
    // Determine output path
    auto output_path = page.output_path.empty() ? 
        config_.output_dir / (page.id + ".html") : page.output_path;
    
    return content_manager_->savePage({page.id, page.title, rendered}, output_path);
}

bool StaticSiteGenerator::copyFile(const std::filesystem::path& source, const std::filesystem::path& destination) {
    try {
        std::filesystem::create_directories(destination.parent_path());
        std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool StaticSiteGenerator::copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination) {
    try {
        std::filesystem::copy(source, destination, 
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string StaticSiteGenerator::markdownToHtml(const std::string& markdown) const {
    // Simple markdown to HTML conversion
    // This is a basic implementation - in a real-world scenario, you'd use a proper markdown library
    std::string html = markdown;
    
    // Convert headers
    std::regex h1_regex(R"(^# (.+)$)", std::regex_constants::multiline);
    html = std::regex_replace(html, h1_regex, "<h1>$1</h1>");
    
    std::regex h2_regex(R"(^## (.+)$)", std::regex_constants::multiline);
    html = std::regex_replace(html, h2_regex, "<h2>$1</h2>");
    
    std::regex h3_regex(R"(^### (.+)$)", std::regex_constants::multiline);
    html = std::regex_replace(html, h3_regex, "<h3>$1</h3>");
    
    // Convert paragraphs (simple approach)
    std::regex para_regex(R"(\n\n)");
    html = std::regex_replace(html, para_regex, "</p>\n<p>");
    html = "<p>" + html + "</p>";
    
    // Convert line breaks
    std::regex br_regex(R"(\n)");
    html = std::regex_replace(html, br_regex, "<br>\n");
    
    return html;
}

// Website implementation
Website::Website(const WebsiteConfig& config) : config_(config) {}
Website::~Website() = default;

bool Website::initialize() {
    if (!setupDirectories()) {
        return false;
    }
    
    // Initialize components
    content_manager_ = std::make_shared<ContentManager>(config_);
    template_engine_ = std::make_shared<TemplateEngine>();
    generator_ = std::make_shared<StaticSiteGenerator>(config_);
    
    // Wire components together
    generator_->setContentManager(content_manager_);
    generator_->setTemplateEngine(template_engine_);
    
    // Load default templates and content
    if (!loadDefaultTemplates()) {
        return false;
    }
    
    // Load existing content
    if (!content_manager_->loadPagesFromDirectory(config_.source_dir)) {
        // This is not necessarily an error if the directory doesn't exist yet
    }
    
    initialized_ = true;
    return true;
}

bool Website::generateSite() {
    if (!initialized_) {
        return false;
    }
    
    return generator_->generateSite();
}

bool Website::serveDevelopmentSite(int port) {
    // This would implement a simple HTTP server for development
    // For now, just log that this feature would be implemented
    g_website_logger.log("Development server would be started on port " + std::to_string(port), 
                        "", "website", LogLevel::INFO);
    return true;
}

bool Website::watchForChanges(bool enable) {
    watching_ = enable;
    if (enable) {
        g_website_logger.log("File watching enabled", "", "website", LogLevel::INFO);
    } else {
        g_website_logger.log("File watching disabled", "", "website", LogLevel::INFO);
    }
    return true;
}

bool Website::updateConfig(const WebsiteConfig& config) {
    config_ = config;
    if (content_manager_) {
        content_manager_->updateConfig(config);
    }
    if (generator_) {
        generator_->updateConfig(config);
    }
    return true;
}

StaticSiteGenerator::GenerationStats Website::getGenerationStats() const {
    return generator_ ? generator_->getLastGenerationStats() : StaticSiteGenerator::GenerationStats{};
}

bool Website::setupDirectories() {
    try {
        std::filesystem::create_directories(config_.source_dir);
        std::filesystem::create_directories(config_.output_dir);
        std::filesystem::create_directories(config_.templates_dir);
        std::filesystem::create_directories(config_.assets_dir);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Website::loadDefaultTemplates() {
    if (!template_engine_) {
        return false;
    }
    
    // Create a simple default HTML template
    std::string default_html_template = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{title}} - {{site_title}}</title>
    <meta name="description" content="{{site_description}}">
</head>
<body>
    <header>
        <h1>{{site_title}}</h1>
        <nav>
            <!-- Navigation would go here -->
        </nav>
    </header>
    <main>
        <h1>{{title}}</h1>
        {{content}}
    </main>
    <footer>
        <p>&copy; 2024 {{site_title}}. All rights reserved.</p>
    </footer>
</body>
</html>)";
    
    // Store templates directly in memory for default templates
    template_engine_->loadTemplate("html", config_.templates_dir / "default.html");
    template_engine_->loadTemplate("markdown", config_.templates_dir / "default.html");
    
    // Set global variables
    template_engine_->setGlobalVariable("site_title", config_.site_title);
    template_engine_->setGlobalVariable("site_description", config_.site_description);
    template_engine_->setGlobalVariable("base_url", config_.base_url);
    
    // Save default template to file if it doesn't exist
    auto default_template_path = config_.templates_dir / "default.html";
    if (!std::filesystem::exists(default_template_path)) {
        try {
            std::ofstream file(default_template_path);
            if (file.is_open()) {
                file << default_html_template;
            }
        } catch (const std::exception&) {
            // Log but don't fail initialization
            g_website_logger.log("Could not save default template to file", "", "website", LogLevel::WARNING);
        }
    }
    
    return true;
}

} // namespace elizaos
