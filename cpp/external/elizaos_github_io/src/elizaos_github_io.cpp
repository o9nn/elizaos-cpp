#include "elizaos/elizaos_github_io.hpp"
#include "elizaos/agentlogger.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <cerrno>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <system_error>

#ifdef _WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace elizaos {

// Global logger instance for the module
static AgentLogger g_github_logger;

// MarkdownProcessor implementation
MarkdownProcessor::MarkdownProcessor() = default;
MarkdownProcessor::~MarkdownProcessor() = default;

std::string MarkdownProcessor::markdownToHtml(const std::string& markdown) const {
    std::string html = markdown;
    
    // Process code blocks first (to protect them from other transformations)
    html = processCodeBlocks(html);
    
    // Process headers (using ECMAScript regex which handles ^ and $ correctly)
    std::regex h6_regex(R"((?:^|\n)###### (.+)(?=\n|$))");
    html = std::regex_replace(html, h6_regex, "<h6>$1</h6>");
    
    std::regex h5_regex(R"((?:^|\n)##### (.+)(?=\n|$))");
    html = std::regex_replace(html, h5_regex, "<h5>$1</h5>");
    
    std::regex h4_regex(R"((?:^|\n)#### (.+)(?=\n|$))");
    html = std::regex_replace(html, h4_regex, "<h4>$1</h4>");
    
    std::regex h3_regex(R"((?:^|\n)### (.+)(?=\n|$))");
    html = std::regex_replace(html, h3_regex, "<h3>$1</h3>");
    
    std::regex h2_regex(R"((?:^|\n)## (.+)(?=\n|$))");
    html = std::regex_replace(html, h2_regex, "<h2>$1</h2>");
    
    std::regex h1_regex(R"((?:^|\n)# (.+)(?=\n|$))");
    html = std::regex_replace(html, h1_regex, "<h1>$1</h1>");
    
    // Process links and images
    html = processLinks(html);
    html = processImages(html);
    
    // Process inline code
    html = processInlineCode(html);
    
    // Process GitHub-specific features
    html = processTaskLists(html);
    html = processGitHubEmojis(html);
    
    return html;
}

std::string MarkdownProcessor::processCodeBlocks(const std::string& markdown) const {
    std::string result = markdown;
    std::regex code_block_regex(R"(```(\w+)?\n([\s\S]*?)\n```)");
    std::smatch matches;
    
    std::string::const_iterator start = result.cbegin();
    std::stringstream output;
    
    while (std::regex_search(start, result.cend(), matches, code_block_regex)) {
        output << std::string(start, matches[0].first);
        
        std::string language = matches[1].str();
        std::string code = matches[2].str();
        
        output << "<pre><code";
        if (!language.empty()) {
            output << " class=\"language-" << language << "\"";
        }
        output << ">" << escapeHtml(code) << "</code></pre>";
        
        start = matches[0].second;
    }
    output << std::string(start, result.cend());
    
    return output.str();
}

std::string MarkdownProcessor::processInlineCode(const std::string& content) const {
    std::regex inline_code_regex(R"(`([^`]+)`)");
    return std::regex_replace(content, inline_code_regex, "<code>$1</code>");
}

std::string MarkdownProcessor::processLinks(const std::string& content) const {
    // Process markdown links [text](url)
    std::regex link_regex(R"(\[([^\]]+)\]\(([^)]+)\))");
    return std::regex_replace(content, link_regex, "<a href=\"$2\">$1</a>");
}

std::string MarkdownProcessor::processImages(const std::string& content) const {
    // Process markdown images ![alt](url)
    std::regex image_regex(R"(!\[([^\]]*)\]\(([^)]+)\))");
    return std::regex_replace(content, image_regex, "<img alt=\"$1\" src=\"$2\" />");
}

std::string MarkdownProcessor::generateTableOfContents(const std::string& markdown) const {
    auto headings = extractHeadings(markdown);
    if (headings.empty()) {
        return "";
    }
    
    std::stringstream toc;
    toc << "<div class=\"table-of-contents\">\n";
    toc << "<h2>Table of Contents</h2>\n";
    toc << "<ul>\n";
    
    for (const auto& heading : headings) {
        std::string anchor = generateAnchorId(heading);
        toc << "<li><a href=\"#" << anchor << "\">" << heading << "</a></li>\n";
    }
    
    toc << "</ul>\n";
    toc << "</div>\n";
    
    return toc.str();
}

std::vector<std::string> MarkdownProcessor::extractHeadings(const std::string& markdown) const {
    std::vector<std::string> headings;
    std::regex heading_regex(R"((?:^|\n)#{1,6}\s+(.+)(?=\n|$))");
    std::sregex_iterator iter(markdown.begin(), markdown.end(), heading_regex);
    std::sregex_iterator end;
    
    while (iter != end) {
        headings.push_back((*iter)[1].str());
        ++iter;
    }
    
    return headings;
}

std::unordered_map<std::string, std::string> MarkdownProcessor::parseFrontmatter(const std::string& content) const {
    std::unordered_map<std::string, std::string> frontmatter;
    
    if (content.substr(0, 3) != "---") {
        return frontmatter;
    }
    
    size_t end_pos = content.find("\n---\n", 3);
    if (end_pos == std::string::npos) {
        return frontmatter;
    }
    
    std::string yaml_content = content.substr(3, end_pos - 3);
    std::istringstream stream(yaml_content);
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
            
            frontmatter[key] = value;
        }
    }
    
    return frontmatter;
}

std::string MarkdownProcessor::stripFrontmatter(const std::string& content) const {
    if (content.substr(0, 3) != "---") {
        return content;
    }
    
    size_t end_pos = content.find("\n---\n", 3);
    if (end_pos == std::string::npos) {
        return content;
    }
    
    return content.substr(end_pos + 5);
}

std::string MarkdownProcessor::processGitHubEmojis(const std::string& content) const {
    // Simple emoji processing - in real implementation you'd have a full emoji std::map
    std::string result = content;
    result = std::regex_replace(result, std::regex(R"(:checkmark:)"), "✅");
    result = std::regex_replace(result, std::regex(R"(:warning:)"), "⚠️");
    result = std::regex_replace(result, std::regex(R"(:error:)"), "❌");
    result = std::regex_replace(result, std::regex(R"(:rocket:)"), "🚀");
    return result;
}

std::string MarkdownProcessor::processTaskLists(const std::string& content) const {
    std::string result = content;
    result = std::regex_replace(result, std::regex(R"(- \[x\] (.+))"), "<li class=\"task-list-item\"><input type=\"checkbox\" checked disabled> $1</li>");
    result = std::regex_replace(result, std::regex(R"(- \[ \] (.+))"), "<li class=\"task-list-item\"><input type=\"checkbox\" disabled> $1</li>");
    return result;
}

std::string MarkdownProcessor::processMentions(const std::string& content) const {
    std::regex mention_regex(R"(@(\w+))");
    return std::regex_replace(content, mention_regex, "<a href=\"https://github.com/$1\" class=\"mention\">@$1</a>");
}

std::string MarkdownProcessor::escapeHtml(const std::string& text) const {
    std::string result = text;
    result = std::regex_replace(result, std::regex("&"), "&amp;");
    result = std::regex_replace(result, std::regex("<"), "&lt;");
    result = std::regex_replace(result, std::regex(">"), "&gt;");
    result = std::regex_replace(result, std::regex("\""), "&quot;");
    result = std::regex_replace(result, std::regex("'"), "&#39;");
    return result;
}

std::string MarkdownProcessor::generateAnchorId(const std::string& heading) const {
    std::string anchor = heading;
    std::transform(anchor.begin(), anchor.end(), anchor.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(anchor.begin(), anchor.end(), anchor.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    std::replace(anchor.begin(), anchor.end(), ' ', '-');
    
    // Remove non-alphanumeric characters except hyphens
    anchor.erase(std::remove_if(anchor.begin(), anchor.end(), 
        [](char c) { return !std::isalnum(c) && c != '-'; }), anchor.end());
    
    return anchor;
}

// DocumentationGenerator implementation
DocumentationGenerator::DocumentationGenerator(const GitHubPagesConfig& config) : config_(config) {
    markdown_processor_ = std::make_shared<MarkdownProcessor>();
}

DocumentationGenerator::~DocumentationGenerator() = default;

bool DocumentationGenerator::generateDocumentation() {
    try {
        // Load default templates
        if (!loadDefaultTemplates()) {
            return false;
        }
        
        // Process markdown files from docs directory
        if (!processMarkdownFiles(config_.docs_dir)) {
            return false;
        }
        
        // Generate navigation
        if (!generateNavigation()) {
            return false;
        }
        
        // Generate API documentation from source code
        generateApiDocs(std::filesystem::current_path() / "include");
        
        // Copy assets
        copyAssets(config_.docs_dir / "assets");
        
        g_github_logger.log("Documentation generated successfully with " + std::to_string(pages_.size()) + " pages",
                           "", "elizaos_github_io", LogLevel::INFO);
        
        return true;
    } catch (const std::exception& e) {
        g_github_logger.log("Failed to generate documentation: " + std::string(e.what()),
                           "", "elizaos_github_io", LogLevel::ERROR);
        return false;
    }
}

bool DocumentationGenerator::generateApiDocs(const std::filesystem::path& source_dir) {
    try {
        auto code_docs = extractCodeDocumentation(source_dir);
        
        for (const auto& code_doc : code_docs) {
            std::string content = generateClassDocumentation(code_doc);
            
            DocumentationPage page("API: " + code_doc.class_name, content);
            page.tags.push_back("api");
            page.frontmatter["layout"] = "api";
            page.frontmatter["category"] = "API Reference";
            page.output_path = config_.output_dir / "api" / (code_doc.class_name + ".html");
            
            addPage(page);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool DocumentationGenerator::generateUserGuide(const std::filesystem::path& /* docs_dir */) {
    // This would scan for user guide markdown files and process them
    g_github_logger.log("User guide generation completed", "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool DocumentationGenerator::generateChangeLog(const std::filesystem::path& /* repo_dir */) {
    // This would generate a changelog from git history
    g_github_logger.log("Changelog generation completed", "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool DocumentationGenerator::addPage(const DocumentationPage& page) {
    pages_.push_back(page);
    return true;
}

bool DocumentationGenerator::removePage(const std::string& page_path) {
    auto it = std::remove_if(pages_.begin(), pages_.end(),
        [&page_path](const DocumentationPage& page) {
            return page.output_path == page_path;
        });
    
    if (it != pages_.end()) {
        pages_.erase(it, pages_.end());
        return true;
    }
    return false;
}

std::vector<DocumentationPage> DocumentationGenerator::getAllPages() const {
    return pages_;
}

std::vector<DocumentationPage> DocumentationGenerator::getPagesByTag(const std::string& tag) const {
    std::vector<DocumentationPage> result;
    for (const auto& page : pages_) {
        if (std::find(page.tags.begin(), page.tags.end(), tag) != page.tags.end()) {
            result.push_back(page);
        }
    }
    return result;
}

bool DocumentationGenerator::generateNavigation() {
    navigation_root_.title = "Documentation";
    navigation_root_.url = "/";
    
    // Sort pages by category and title
    std::sort(pages_.begin(), pages_.end(), [](const DocumentationPage& a, const DocumentationPage& b) {
        auto a_category = a.frontmatter.count("category") ? a.frontmatter.at("category") : "General";
        auto b_category = b.frontmatter.count("category") ? b.frontmatter.at("category") : "General";
        
        if (a_category != b_category) {
            return a_category < b_category;
        }
        return a.title < b.title;
    });
    
    // Build navigation tree
    std::string current_category;
    NavigationItem* current_category_item = nullptr;
    
    for (const auto& page : pages_) {
        std::string category = page.frontmatter.count("category") ? 
            page.frontmatter.at("category") : "General";
        
        if (category != current_category) {
            current_category = category;
            NavigationItem category_item(category, "#", static_cast<int>(navigation_root_.children.size()));
            navigation_root_.children.push_back(category_item);
            current_category_item = &navigation_root_.children.back();
        }
        
        if (current_category_item) {
            std::string url = page.output_path.filename().replace_extension("").string() + ".html";
            NavigationItem page_item(page.title, "/" + url, static_cast<int>(current_category_item->children.size()));
            current_category_item->children.push_back(page_item);
        }
    }
    
    return true;
}

NavigationItem DocumentationGenerator::buildNavigationTree() const {
    return navigation_root_;
}

std::string DocumentationGenerator::renderNavigationHtml(const NavigationItem& nav) const {
    std::stringstream html;
    html << "<nav class=\"documentation-nav\">\n";
    html << "<ul>\n";
    
    for (const auto& item : nav.children) {
        html << "<li>\n";
        html << "<a href=\"" << item.url << "\">" << item.title << "</a>\n";
        
        if (!item.children.empty()) {
            html << "<ul>\n";
            for (const auto& child : item.children) {
                html << "<li><a href=\"" << child.url << "\">" << child.title << "</a></li>\n";
            }
            html << "</ul>\n";
        }
        
        html << "</li>\n";
    }
    
    html << "</ul>\n";
    html << "</nav>\n";
    
    return html.str();
}

bool DocumentationGenerator::loadTheme(const std::string& theme_name) {
    // Load a predefined theme
    g_github_logger.log("Loading theme: " + theme_name, "", "elizaos_github_io", LogLevel::INFO);
    return loadDefaultTemplates();
}

bool DocumentationGenerator::loadCustomTheme(const std::filesystem::path& theme_dir) {
    // Load custom theme from directory
    g_github_logger.log("Loading custom theme from: " + theme_dir.string(),
                       "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

std::string DocumentationGenerator::applyTemplate(const std::string& template_name, const DocumentationPage& page) const {
    auto it = templates_.find(template_name);
    if (it == templates_.end()) {
        return page.content;
    }
    
    std::string result = it->second;
    
    // Simple template variable substitution
    result = std::regex_replace(result, std::regex(R"(\{\{title\}\})"), page.title);
    result = std::regex_replace(result, std::regex(R"(\{\{content\}\})"), page.content);
    
    // Add navigation
    auto nav_html = renderNavigationHtml(navigation_root_);
    result = std::regex_replace(result, std::regex(R"(\{\{navigation\}\})"), nav_html);
    
    return result;
}

std::vector<DocumentationGenerator::CodeDocumentation> DocumentationGenerator::extractCodeDocumentation(const std::filesystem::path& source_dir) const {
    std::vector<CodeDocumentation> docs;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(source_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".hpp") {
                auto class_info = extractClassFromHeader(entry.path());
                if (!class_info.empty()) {
                    CodeDocumentation doc;
                    doc.class_name = class_info;
                    doc.source_file = entry.path();
                    doc.description = "C++ class documentation for " + class_info;
                    docs.push_back(doc);
                }
            }
        }
    } catch (const std::exception&) {
        // Continue processing other files
    }
    
    return docs;
}

std::string DocumentationGenerator::generateClassDocumentation(const CodeDocumentation& code_doc) const {
    std::stringstream doc;
    doc << "# " << code_doc.class_name << "\n\n";
    doc << code_doc.description << "\n\n";
    doc << "**Source File:** `" << code_doc.source_file.filename().string() << "`\n\n";
    
    if (!code_doc.methods.empty()) {
        doc << "## Methods\n\n";
        for (const auto& method : code_doc.methods) {
            doc << "- `" << method << "`\n";
        }
        doc << "\n";
    }
    
    if (!code_doc.properties.empty()) {
        doc << "## Properties\n\n";
        for (const auto& property : code_doc.properties) {
            doc << "- `" << property << "`\n";
        }
        doc << "\n";
    }
    
    return doc.str();
}

bool DocumentationGenerator::loadDefaultTemplates() {
    // Load a simple default template
    std::string default_template = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{title}} - ElizaOS Documentation</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; }
        .container { max-width: 1200px; margin: 0 auto; padding: 20px; }
        .nav { background: #f8f9fa; padding: 20px; margin-bottom: 20px; }
        .content { line-height: 1.6; }
        pre { background: #f6f8fa; padding: 16px; border-radius: 6px; overflow-x: auto; }
        code { background: #f6f8fa; padding: 2px 4px; border-radius: 3px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="nav">{{navigation}}</div>
        <div class="content">
            <h1>{{title}}</h1>
            {{content}}
        </div>
    </div>
</body>
</html>)";
    
    templates_["default"] = default_template;
    templates_["api"] = default_template;
    
    return true;
}

bool DocumentationGenerator::processMarkdownFiles(const std::filesystem::path& input_dir) {
    if (!std::filesystem::exists(input_dir)) {
        return true; // Not an error if directory doesn't exist
    }
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(input_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".md") {
                std::ifstream file(entry.path());
                if (!file.is_open()) continue;
                
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
                
                auto frontmatter = markdown_processor_->parseFrontmatter(content);
                std::string markdown_content = markdown_processor_->stripFrontmatter(content);
                std::string html_content = markdown_processor_->markdownToHtml(markdown_content);
                
                std::string title = frontmatter.count("title") ? 
                    frontmatter["title"] : entry.path().stem().string();
                
                DocumentationPage page(title, html_content);
                page.markdown_content = markdown_content;
                page.source_path = entry.path();
                page.frontmatter = frontmatter;
                page.last_modified = std::chrono::system_clock::now();
                
                // Set output path
                auto relative_path = std::filesystem::relative(entry.path(), input_dir);
                page.output_path = config_.output_dir / relative_path.replace_extension(".html");
                
                addPage(page);
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool DocumentationGenerator::copyAssets(const std::filesystem::path& assets_dir) {
    if (!std::filesystem::exists(assets_dir)) {
        return true;
    }
    
    try {
        auto target_dir = config_.output_dir / "assets";
        std::filesystem::create_directories(target_dir);
        std::filesystem::copy(assets_dir, target_dir, 
            std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string DocumentationGenerator::extractClassFromHeader(const std::filesystem::path& header_file) const {
    try {
        std::ifstream file(header_file);
        if (!file.is_open()) return "";
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        std::regex class_regex(R"(class\s+(\w+))");
        std::smatch matches;
        
        if (std::regex_search(content, matches, class_regex)) {
            return matches[1].str();
        }
    } catch (const std::exception&) {
        // Continue
    }
    
    return "";
}

std::string DocumentationGenerator::extractDocComment(const std::string& /* source_code */, const std::string& element_name) const {
    // Simple doc comment extraction - would be more sophisticated in practice
    return "Documentation for " + element_name;
}

// GitHubPagesDeployer implementation
GitHubPagesDeployer::GitHubPagesDeployer(const GitHubPagesConfig& config) : config_(config) {
    temp_repo_dir_ = std::filesystem::temp_directory_path() / "elizaos-github-pages";
}

GitHubPagesDeployer::~GitHubPagesDeployer() = default;

bool GitHubPagesDeployer::deploy() {
    if (!validateConfiguration()) {
        return false;
    }
    
    last_deployment_status_ = DeploymentStatus{};
    last_deployment_status_.deployment_time = std::chrono::system_clock::now();
    
    try {
        // Clone or update repository
        if (!cloneRepository(temp_repo_dir_)) {
            last_deployment_status_.errors.push_back("Failed to clone repository");
            return false;
        }
        
        // Copy files to repository
        if (!copyFilesToRepo(config_.output_dir, temp_repo_dir_)) {
            last_deployment_status_.errors.push_back("Failed to copy files to repository");
            return false;
        }
        
        // Commit changes
        if (!commitChanges("Update GitHub Pages")) {
            last_deployment_status_.errors.push_back("Failed to commit changes");
            return false;
        }
        
        // Push to GitHub
        if (!pushToGitHub()) {
            last_deployment_status_.errors.push_back("Failed to push to GitHub");
            return false;
        }
        
        last_deployment_status_.success = true;
        last_deployment_status_.deployment_url = "https://" + config_.repository_owner + 
            ".github.io/" + config_.repository_name;
        
        g_github_logger.log("Successfully deployed to GitHub Pages: " + last_deployment_status_.deployment_url, "", "elizaos_github_io", LogLevel::INFO);
        
        return true;
    } catch (const std::exception& e) {
        last_deployment_status_.errors.push_back(std::string("Deployment error: ") + e.what());
        return false;
    }
}

bool GitHubPagesDeployer::deployDirectory(const std::filesystem::path& source_dir) {
    auto old_output_dir = config_.output_dir;
    config_.output_dir = source_dir;
    bool result = deploy();
    config_.output_dir = old_output_dir;
    return result;
}

bool GitHubPagesDeployer::validateConfiguration() const {
    if (config_.repository_owner.empty() || config_.repository_name.empty()) {
        return false;
    }
    if (config_.access_token.empty()) {
        return false;
    }
    static const std::regex github_name(R"(^[A-Za-z0-9][A-Za-z0-9_.-]*$)");
    return std::regex_match(config_.repository_owner, github_name) &&
           std::regex_match(config_.repository_name, github_name) &&
           validateGitRef(config_.branch) &&
           std::filesystem::is_directory(config_.output_dir);
}

bool GitHubPagesDeployer::cloneRepository(const std::filesystem::path& target_dir) {
    if (target_dir.empty() || config_.repository_owner.empty() ||
        config_.repository_name.empty() || config_.access_token.empty() ||
        !validateGitRef(config_.branch)) {
        return false;
    }

    try {
        // Clean up existing directory
        if (std::filesystem::exists(target_dir)) {
            std::filesystem::remove_all(target_dir);
        }

        if (!target_dir.parent_path().empty()) {
            std::filesystem::create_directories(target_dir.parent_path());
        }

        const std::string repo_url = "https://github.com/" + config_.repository_owner +
            "/" + config_.repository_name + ".git";
        const std::unordered_map<std::string, std::string> auth_env = {
            {"ELIZA_GITHUB_HEADER", "Authorization: Bearer " + config_.access_token}
        };

        auto result = executeCommand(
            {"git", "--config-env=http.extraHeader=ELIZA_GITHUB_HEADER", "clone",
             "--single-branch", "--branch", config_.branch, repo_url, target_dir.string()},
            {}, auth_env);

        // A fresh repository may not yet have the configured Pages branch. In
        // that case clone the default branch and create the Pages branch locally.
        if (!result.succeeded()) {
            std::error_code cleanup_error;
            std::filesystem::remove_all(target_dir, cleanup_error);
            result = executeCommand(
                {"git", "--config-env=http.extraHeader=ELIZA_GITHUB_HEADER", "clone",
                 repo_url, target_dir.string()}, {}, auth_env);
            if (!result.succeeded()) {
                last_deployment_status_.errors.push_back("git clone failed: " + result.output);
                return false;
            }
            result = executeCommand({"git", "checkout", "-B", config_.branch}, target_dir);
            if (!result.succeeded()) {
                last_deployment_status_.errors.push_back("failed to create deployment branch: " + result.output);
                return false;
            }
        }

        temp_repo_dir_ = target_dir;
        g_github_logger.log("Repository cloned to: " + target_dir.string(),
                           "", "elizaos_github_io", LogLevel::INFO);
        return true;
    } catch (const std::exception& e) {
        last_deployment_status_.errors.push_back(std::string("clone error: ") + e.what());
        return false;
    }
}

bool GitHubPagesDeployer::copyFilesToRepo(const std::filesystem::path& source_dir, const std::filesystem::path& repo_dir) {
    try {
        if (!std::filesystem::is_directory(source_dir) ||
            !std::filesystem::is_directory(repo_dir / ".git")) {
            return false;
        }

        const auto source = std::filesystem::weakly_canonical(source_dir);
        const auto destination = std::filesystem::weakly_canonical(repo_dir);
        auto is_prefix = [](const std::filesystem::path& prefix, const std::filesystem::path& value) {
            return std::mismatch(prefix.begin(), prefix.end(), value.begin(), value.end()).first == prefix.end();
        };
        if (source == destination || is_prefix(source, destination) || is_prefix(destination, source)) {
            return false;
        }

        // Remove stale deployment content while preserving repository metadata.
        for (const auto& entry : std::filesystem::directory_iterator(repo_dir)) {
            if (entry.path().filename() != ".git") {
                std::filesystem::remove_all(entry.path());
            }
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(source_dir)) {
            auto relative_path = std::filesystem::relative(entry.path(), source_dir);
            if (!relative_path.empty() && *relative_path.begin() == ".git") {
                continue;
            }
            if (entry.is_symlink()) {
                last_deployment_status_.errors.push_back(
                    "refusing to deploy symbolic link: " + entry.path().string());
                return false;
            }

            auto target_path = repo_dir / relative_path;
            if (entry.is_directory()) {
                std::filesystem::create_directories(target_path);
            } else if (entry.is_regular_file()) {
                std::filesystem::create_directories(target_path.parent_path());
                std::filesystem::copy_file(entry.path(), target_path,
                    std::filesystem::copy_options::overwrite_existing);
            }
        }
        return true;
    } catch (const std::exception& e) {
        last_deployment_status_.errors.push_back(std::string("copy error: ") + e.what());
        return false;
    }
}

GitHubPagesDeployer::CommandResult GitHubPagesDeployer::executeCommand(
    const std::vector<std::string>& arguments,
    const std::filesystem::path& working_dir,
    const std::unordered_map<std::string, std::string>& environment) const {
    CommandResult result;
    if (arguments.empty() || arguments.front().empty()) {
        result.output = "empty command";
        return result;
    }
    if (!working_dir.empty() && !std::filesystem::is_directory(working_dir)) {
        result.output = "working directory does not exist";
        return result;
    }

    g_github_logger.log("Executing process " + arguments.front() + " (" +
                        std::to_string(arguments.size()) + " arguments)",
                        "", "elizaos_github_io", LogLevel::INFO);

#ifdef _WIN32
    // Windows fallback. Reject cmd.exe metacharacters before constructing the
    // command line; POSIX builds below never invoke a shell.
    auto safe_arg = [](const std::string& arg) {
        return arg.find_first_of("\r\n&|<>^%\"") == std::string::npos;
    };
    std::ostringstream command;
    for (const auto& argument : arguments) {
        if (!safe_arg(argument)) {
            result.output = "unsafe command argument";
            return result;
        }
        if (command.tellp() > 0) command << ' ';
        command << '"' << argument << '"';
    }
    std::filesystem::path previous;
    if (!working_dir.empty()) {
        previous = std::filesystem::current_path();
        std::filesystem::current_path(working_dir);
    }
    std::unordered_map<std::string, std::string> old_environment;
    for (const auto& [key, value] : environment) {
        const char* old = std::getenv(key.c_str());
        old_environment[key] = old ? old : "";
        _putenv_s(key.c_str(), value.c_str());
    }
    FILE* pipe = _popen((command.str() + " 2>&1").c_str(), "r");
    if (pipe) {
        std::array<char, 4096> buffer{};
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe)) {
            result.output += buffer.data();
        }
        result.exit_code = _pclose(pipe);
    } else {
        result.output = "unable to start process";
    }
    for (const auto& [key, value] : old_environment) {
        _putenv_s(key.c_str(), value.c_str());
    }
    if (!working_dir.empty()) std::filesystem::current_path(previous);
#else
    int output_pipe[2];
    if (pipe(output_pipe) != 0) {
        result.output = std::string("pipe failed: ") + std::strerror(errno);
        return result;
    }

    const pid_t child = fork();
    if (child < 0) {
        close(output_pipe[0]);
        close(output_pipe[1]);
        result.output = std::string("fork failed: ") + std::strerror(errno);
        return result;
    }
    if (child == 0) {
        close(output_pipe[0]);
        dup2(output_pipe[1], STDOUT_FILENO);
        dup2(output_pipe[1], STDERR_FILENO);
        close(output_pipe[1]);
        if (!working_dir.empty() && chdir(working_dir.c_str()) != 0) {
            _exit(126);
        }
        for (const auto& [key, value] : environment) {
            if (setenv(key.c_str(), value.c_str(), 1) != 0) {
                _exit(126);
            }
        }
        std::vector<char*> argv;
        argv.reserve(arguments.size() + 1);
        for (const auto& argument : arguments) {
            argv.push_back(const_cast<char*>(argument.c_str()));
        }
        argv.push_back(nullptr);
        execvp(argv.front(), argv.data());
        _exit(127);
    }

    close(output_pipe[1]);
    std::array<char, 4096> buffer{};
    while (true) {
        const ssize_t count = read(output_pipe[0], buffer.data(), buffer.size());
        if (count > 0) {
            result.output.append(buffer.data(), static_cast<std::size_t>(count));
        } else if (count == 0) {
            break;
        } else if (errno != EINTR) {
            break;
        }
    }
    close(output_pipe[0]);

    int status = 0;
    while (waitpid(child, &status, 0) < 0 && errno == EINTR) {}
    if (WIFEXITED(status)) {
        result.exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        result.exit_code = 128 + WTERMSIG(status);
    }
#endif
    return result;
}

bool GitHubPagesDeployer::validateGitRef(const std::string& ref_name) const {
    if (ref_name.empty()) return false;
    return executeCommand({"git", "check-ref-format", "--branch", ref_name}).succeeded();
}

bool GitHubPagesDeployer::commitChanges(const std::string& commit_message) {
    if (commit_message.empty() || !std::filesystem::is_directory(temp_repo_dir_ / ".git")) {
        return false;
    }
    if (!executeCommand({"git", "add", "--all"}, temp_repo_dir_).succeeded()) {
        return false;
    }

    const auto staged = executeCommand({"git", "diff", "--cached", "--quiet"}, temp_repo_dir_);
    if (staged.exit_code == 0) {
        last_deployment_status_.warnings.push_back("No deployment changes to commit");
        return true;
    }
    if (staged.exit_code != 1) {
        last_deployment_status_.errors.push_back("Unable to inspect staged changes: " + staged.output);
        return false;
    }

    if (!executeCommand({"git", "config", "user.name"}, temp_repo_dir_).succeeded()) {
        if (!executeCommand({"git", "config", "user.name", "ElizaOS"}, temp_repo_dir_).succeeded()) {
            return false;
        }
    }
    if (!executeCommand({"git", "config", "user.email"}, temp_repo_dir_).succeeded()) {
        if (!executeCommand({"git", "config", "user.email", "elizaos@localhost"}, temp_repo_dir_).succeeded()) {
            return false;
        }
    }

    const auto committed = executeCommand({"git", "commit", "--message", commit_message}, temp_repo_dir_);
    if (!committed.succeeded()) {
        last_deployment_status_.errors.push_back("git commit failed: " + committed.output);
        return false;
    }
    const auto revision = executeCommand({"git", "rev-parse", "HEAD"}, temp_repo_dir_);
    if (revision.succeeded()) {
        last_deployment_status_.commit_sha = revision.output;
        while (!last_deployment_status_.commit_sha.empty() &&
               std::isspace(static_cast<unsigned char>(last_deployment_status_.commit_sha.back()))) {
            last_deployment_status_.commit_sha.pop_back();
        }
    }
    return true;
}

bool GitHubPagesDeployer::pushToGitHub() {
    if (!validateGitRef(config_.branch) || config_.access_token.empty() ||
        !std::filesystem::is_directory(temp_repo_dir_ / ".git")) {
        return false;
    }
    const std::unordered_map<std::string, std::string> auth_env = {
        {"ELIZA_GITHUB_HEADER", "Authorization: Bearer " + config_.access_token}
    };
    const auto result = executeCommand(
        {"git", "--config-env=http.extraHeader=ELIZA_GITHUB_HEADER", "push",
         "--set-upstream", "origin", config_.branch}, temp_repo_dir_, auth_env);
    if (!result.succeeded()) {
        last_deployment_status_.errors.push_back("git push failed: " + result.output);
    }
    return result.succeeded();
}

bool GitHubPagesDeployer::createGitHubPagesRepo() {
    std::string response;
    std::string data = "{\"name\":\"" + config_.repository_name + "\",\"auto_init\":true,\"description\":\"GitHub Pages repository\"}";
    if (!makeHttpRequest("POST", buildGitHubApiUrl("user/repos"), data, response)) {
        last_deployment_status_.errors.push_back("Failed to create GitHub Pages repository");
        return false;
    }
    g_github_logger.log("GitHub Pages repository created: " + config_.repository_owner + "/" + config_.repository_name,
                       "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool GitHubPagesDeployer::updateGitHubPagesRepo() {
    std::string response;
    std::string data = "{\"description\":\"GitHub Pages - updated\",\"has_pages\":true}";
    std::string endpoint = "repos/" + config_.repository_owner + "/" + config_.repository_name;
    if (!makeHttpRequest("PATCH", buildGitHubApiUrl(endpoint), data, response)) {
        last_deployment_status_.warnings.push_back("Failed to update GitHub Pages repository settings");
        return false;
    }
    g_github_logger.log("GitHub Pages repository updated: " + config_.repository_owner + "/" + config_.repository_name,
                       "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool GitHubPagesDeployer::initializeGitRepo(const std::filesystem::path& repo_dir) {
    try {
        if (repo_dir.empty() || !validateGitRef(config_.branch)) {
            return false;
        }
        std::filesystem::create_directories(repo_dir);
        auto result = executeCommand({"git", "init"}, repo_dir);
        if (!result.succeeded()) {
            last_deployment_status_.errors.push_back("Failed to initialize git repository at: " + repo_dir.string());
            return false;
        }
        result = executeCommand({"git", "checkout", "-B", config_.branch}, repo_dir);
        if (!result.succeeded()) {
            last_deployment_status_.errors.push_back("Failed to initialize branch: " + result.output);
            return false;
        }
        temp_repo_dir_ = repo_dir;
        g_github_logger.log("Git repository initialized at: " + repo_dir.string(),
                           "", "elizaos_github_io", LogLevel::INFO);
        return true;
    } catch (const std::exception& e) {
        last_deployment_status_.errors.push_back(std::string("Git init error: ") + e.what());
        return false;
    }
}

bool GitHubPagesDeployer::createBranch(const std::string& branch_name) {
    if (!validateGitRef(branch_name) || !std::filesystem::is_directory(temp_repo_dir_ / ".git")) {
        return false;
    }
    auto result = executeCommand({"git", "checkout", "-b", branch_name}, temp_repo_dir_);
    if (!result.succeeded()) {
        last_deployment_status_.errors.push_back("Failed to create branch: " + branch_name);
        return false;
    }
    g_github_logger.log("Branch created: " + branch_name, "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool GitHubPagesDeployer::switchToBranch(const std::string& branch_name) {
    if (!validateGitRef(branch_name) || !std::filesystem::is_directory(temp_repo_dir_ / ".git")) {
        return false;
    }
    auto result = executeCommand({"git", "checkout", branch_name}, temp_repo_dir_);
    if (!result.succeeded()) {
        last_deployment_status_.errors.push_back("Failed to switch to branch: " + branch_name);
        return false;
    }
    g_github_logger.log("Switched to branch: " + branch_name, "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool GitHubPagesDeployer::enableGitHubPages() {
    std::string response;
    std::string endpoint = "repos/" + config_.repository_owner + "/" + config_.repository_name + "/pages";
    std::string data = "{\"source\":{\"branch\":\"" + config_.branch + "\",\"path\":\"/\"}}";
    if (!makeHttpRequest("POST", buildGitHubApiUrl(endpoint), data, response)) {
        last_deployment_status_.errors.push_back("Failed to enable GitHub Pages");
        return false;
    }
    g_github_logger.log("GitHub Pages enabled for: " + config_.repository_owner + "/" + config_.repository_name,
                       "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool GitHubPagesDeployer::updateGitHubPagesSettings() {
    std::string response;
    std::string endpoint = "repos/" + config_.repository_owner + "/" + config_.repository_name + "/pages";
    std::string data = "{\"source\":{\"branch\":\"" + config_.branch + "\",\"path\":\"/\"}}";
    if (!config_.cname_domain.empty()) {
        data = "{\"source\":{\"branch\":\"" + config_.branch + "\",\"path\":\"/\"},\"cname\":\"" + config_.cname_domain + "\"}";
    }
    if (!makeHttpRequest("PUT", buildGitHubApiUrl(endpoint), data, response)) {
        last_deployment_status_.warnings.push_back("Failed to update GitHub Pages settings");
        return false;
    }
    g_github_logger.log("GitHub Pages settings updated", "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool GitHubPagesDeployer::checkDeploymentStatus() {
    std::string response;
    std::string endpoint = "repos/" + config_.repository_owner + "/" + config_.repository_name + "/pages/builds/latest";
    if (!makeHttpRequest("GET", buildGitHubApiUrl(endpoint), "", response)) {
        last_deployment_status_.warnings.push_back("Failed to retrieve deployment status");
        return false;
    }
    // Look for "built" status in the response
    bool built = response.find("\"built\"") != std::string::npos ||
                 response.find("\"status\":\"built\"") != std::string::npos;
    if (built) {
        last_deployment_status_.deployment_url = "https://" + config_.repository_owner +
            ".github.io/" + config_.repository_name;
        g_github_logger.log("Deployment status: built. URL: " + last_deployment_status_.deployment_url,
                           "", "elizaos_github_io", LogLevel::INFO);
    } else {
        g_github_logger.log("Deployment status: in progress or unavailable",
                           "", "elizaos_github_io", LogLevel::INFO);
    }
    return true;
}

bool GitHubPagesDeployer::makeHttpRequest(const std::string& method, const std::string& url,
                                          const std::string& data, std::string& response) const {
    response.clear();
    // Validate HTTP method and destination before launching curl.
    static const std::vector<std::string> allowed_methods = {"GET", "POST", "PUT", "PATCH", "DELETE"};
    if (std::find(allowed_methods.begin(), allowed_methods.end(), method) == allowed_methods.end()) {
        return false;
    }

    // Validate that the URL starts with the expected GitHub API base to prevent injection
    const std::string expected_base = "https://api.github.com/";
    if (url.rfind(expected_base, 0) != 0) {
        return false;
    }

    const bool has_data = !data.empty();
    std::error_code ec;
    std::ostringstream unique_name;
    unique_name << "elizaos_github_" << reinterpret_cast<std::uintptr_t>(this) << "_"
                << std::chrono::steady_clock::now().time_since_epoch().count();
    const auto temp_root = std::filesystem::temp_directory_path();
    const auto body_file = temp_root / (unique_name.str() + ".json");
    const auto header_file = temp_root / (unique_name.str() + ".header");

    auto cleanup = [&]() {
        if (has_data) std::filesystem::remove(body_file, ec);
        std::filesystem::remove(header_file, ec);
    };

    // Write request body to a file and use @filename to avoid embedding
    // user-controlled data in the shell command string
    if (has_data) {
        try {
            std::ofstream out(body_file, std::ios::binary);
            if (!out) { return false; }
            out << data;
            if (!out.good()) { cleanup(); return false; }
        } catch (const std::exception&) {
            cleanup();
            return false;
        }
    }

    // Write the Authorization header to a file and use curl's -H @file syntax
    // so the access token is never embedded directly in the shell command string
    try {
        std::ofstream hout(header_file, std::ios::binary);
        if (!hout) { cleanup(); return false; }
        hout << getAuthorizationHeader() << "\n";
        if (!hout.good()) { cleanup(); return false; }
        std::filesystem::permissions(
            header_file,
            std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
            std::filesystem::perm_options::replace,
            ec);
    } catch (const std::exception&) {
        cleanup();
        return false;
    }

    std::vector<std::string> arguments = {
        "curl", "--silent", "--show-error", "--write-out", "\n%{http_code}",
        "--request", method,
        "--header", "Accept: application/vnd.github+json",
        "--header", "@" + header_file.string(),
        "--header", "X-GitHub-Api-Version: 2022-11-28"
    };
    if (has_data) {
        arguments.insert(arguments.end(), {
            "--header", "Content-Type: application/json",
            "--data-binary", "@" + body_file.string()
        });
    }
    arguments.push_back(url);

    const auto command_result = executeCommand(arguments);
    cleanup();
    if (!command_result.succeeded()) {
        response = command_result.output;
        return false;
    }

    const std::string& raw = command_result.output;

    // The last line from curl -w "\n%{http_code}" is the HTTP status code
    auto last_newline = raw.rfind('\n', raw.size() > 1 ? raw.size() - 2 : 0);
    if (last_newline != std::string::npos) {
        std::string http_code_str = raw.substr(last_newline + 1);
        // Trim trailing whitespace/newline
        while (!http_code_str.empty() && (http_code_str.back() == '\n' || http_code_str.back() == '\r' || http_code_str.back() == ' ')) {
            http_code_str.pop_back();
        }
        response = raw.substr(0, last_newline);
        try {
            int http_code = std::stoi(http_code_str);
            return (http_code >= 200 && http_code < 300);
        } catch (const std::exception&) {
            return false;
        }
    }

    response = raw;
    return false;
}

std::string GitHubPagesDeployer::buildGitHubApiUrl(const std::string& endpoint) const {
    return "https://api.github.com/" + endpoint;
}

std::string GitHubPagesDeployer::getAuthorizationHeader() const {
    return "Authorization: Bearer " + config_.access_token;
}

// ElizaOSGitHubIO main implementation
ElizaOSGitHubIO::ElizaOSGitHubIO(const GitHubPagesConfig& config) : config_(config) {}
ElizaOSGitHubIO::~ElizaOSGitHubIO() = default;

bool ElizaOSGitHubIO::initialize() {
    if (!setupWorkspace()) {
        return false;
    }
    
    if (!validateEnvironment()) {
        return false;
    }
    
    // Initialize components
    doc_generator_ = std::make_shared<DocumentationGenerator>(config_);
    deployer_ = std::make_shared<GitHubPagesDeployer>(config_);
    markdown_processor_ = std::make_shared<MarkdownProcessor>();
    
    initialized_ = true;
    
    g_github_logger.log("ElizaOS GitHub.io module initialized successfully", "", "elizaos_github_io", LogLevel::INFO);
    
    return true;
}

bool ElizaOSGitHubIO::generateAndDeploy() {
    if (!initialized_) {
        return false;
    }
    
    deployment_in_progress_ = true;
    
    bool success = generateDocumentation() && deployToGitHubPages();
    
    deployment_in_progress_ = false;
    
    return success;
}

bool ElizaOSGitHubIO::generateDocumentation() {
    if (!doc_generator_) {
        return false;
    }
    
    g_github_logger.log("Starting documentation generation...", "", "elizaos_github_io", LogLevel::INFO);
    
    return doc_generator_->generateDocumentation();
}

bool ElizaOSGitHubIO::deployToGitHubPages() {
    if (!deployer_) {
        return false;
    }
    
    g_github_logger.log("Starting GitHub Pages deployment...", "", "elizaos_github_io", LogLevel::INFO);
    
    return deployer_->deploy();
}

bool ElizaOSGitHubIO::updateConfig(const GitHubPagesConfig& config) {
    config_ = config;
    if (doc_generator_) {
        doc_generator_->updateConfig(config);
    }
    if (deployer_) {
        deployer_->updateConfig(config);
    }
    return true;
}

GitHubPagesDeployer::DeploymentStatus ElizaOSGitHubIO::getDeploymentStatus() const {
    return deployer_ ? deployer_->getLastDeploymentStatus() : GitHubPagesDeployer::DeploymentStatus{};
}

bool ElizaOSGitHubIO::enableAutoDeployment(const std::string& trigger_branch) {
    auto_deployment_enabled_ = true;
    g_github_logger.log("Auto-deployment enabled for branch: " + trigger_branch, "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool ElizaOSGitHubIO::disableAutoDeployment() {
    auto_deployment_enabled_ = false;
    g_github_logger.log("Auto-deployment disabled", "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool ElizaOSGitHubIO::watchForChanges(const std::filesystem::path& watch_dir) {
    g_github_logger.log("File watching enabled for: " + watch_dir.string(),
                       "", "elizaos_github_io", LogLevel::INFO);
    return true;
}

bool ElizaOSGitHubIO::setupWorkspace() {
    try {
        std::filesystem::create_directories(config_.docs_dir);
        std::filesystem::create_directories(config_.output_dir);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ElizaOSGitHubIO::validateEnvironment() {
    // Check if git is available
    // In a real implementation, we'd check for git executable
    return true;
}

} // namespace elizaos
