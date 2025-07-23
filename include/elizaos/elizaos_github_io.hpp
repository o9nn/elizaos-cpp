#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <chrono>

namespace elizaos {

/**
 * ElizaOS GitHub.io Module - Static site generation and deployment for GitHub Pages
 * Provides automated documentation generation and GitHub Pages deployment
 */

class MarkdownProcessor;
class GitHubPagesDeployer;
class DocumentationGenerator;

/**
 * GitHub Pages configuration
 */
struct GitHubPagesConfig {
    std::string repository_owner;
    std::string repository_name;
    std::string branch = "gh-pages";
    std::string access_token;
    std::filesystem::path docs_dir = "./docs";
    std::filesystem::path output_dir = "./github-pages";
    std::string base_url;
    std::string cname_domain;
    bool use_jekyll = false;
    
    GitHubPagesConfig() = default;
    GitHubPagesConfig(const std::string& owner, const std::string& repo)
        : repository_owner(owner), repository_name(repo) {}
};

/**
 * Documentation page structure
 */
struct DocumentationPage {
    std::string title;
    std::string content;
    std::string markdown_content;
    std::filesystem::path source_path;
    std::filesystem::path output_path;
    std::unordered_map<std::string, std::string> frontmatter;
    std::vector<std::string> tags;
    int order = 0;
    std::chrono::system_clock::time_point last_modified;
    
    DocumentationPage() = default;
    DocumentationPage(const std::string& page_title, const std::string& page_content)
        : title(page_title), content(page_content), 
          last_modified(std::chrono::system_clock::now()) {}
};

/**
 * Navigation structure for documentation
 */
struct NavigationItem {
    std::string title;
    std::string url;
    std::vector<NavigationItem> children;
    int order = 0;
    bool is_external = false;
    
    NavigationItem() = default;
    NavigationItem(const std::string& item_title, const std::string& item_url, int item_order = 0)
        : title(item_title), url(item_url), order(item_order) {}
};

/**
 * Markdown processor for GitHub-flavored markdown
 */
class MarkdownProcessor {
public:
    MarkdownProcessor();
    ~MarkdownProcessor();
    
    // Markdown processing
    std::string markdownToHtml(const std::string& markdown) const;
    std::string processCodeBlocks(const std::string& markdown) const;
    std::string processInlineCode(const std::string& content) const;
    std::string processLinks(const std::string& content) const;
    std::string processImages(const std::string& content) const;
    
    // Table of contents generation
    std::string generateTableOfContents(const std::string& markdown) const;
    std::vector<std::string> extractHeadings(const std::string& markdown) const;
    
    // Frontmatter processing
    std::unordered_map<std::string, std::string> parseFrontmatter(const std::string& content) const;
    std::string stripFrontmatter(const std::string& content) const;
    
    // GitHub-specific features
    std::string processGitHubEmojis(const std::string& content) const;
    std::string processTaskLists(const std::string& content) const;
    std::string processMentions(const std::string& content) const;
    
private:
    bool isCodeBlock(const std::string& line) const;
    std::string escapeHtml(const std::string& text) const;
    std::string generateAnchorId(const std::string& heading) const;
};

/**
 * Documentation generator from source code and markdown files
 */
class DocumentationGenerator {
public:
    DocumentationGenerator(const GitHubPagesConfig& config);
    ~DocumentationGenerator();
    
    // Documentation generation
    bool generateDocumentation();
    bool generateApiDocs(const std::filesystem::path& source_dir);
    bool generateUserGuide(const std::filesystem::path& docs_dir);
    bool generateChangeLog(const std::filesystem::path& repo_dir);
    
    // Page management
    bool addPage(const DocumentationPage& page);
    bool removePage(const std::string& page_path);
    std::vector<DocumentationPage> getAllPages() const;
    std::vector<DocumentationPage> getPagesByTag(const std::string& tag) const;
    
    // Navigation
    bool generateNavigation();
    NavigationItem buildNavigationTree() const;
    std::string renderNavigationHtml(const NavigationItem& nav) const;
    
    // Templates and themes
    bool loadTheme(const std::string& theme_name);
    bool loadCustomTheme(const std::filesystem::path& theme_dir);
    std::string applyTemplate(const std::string& template_name, const DocumentationPage& page) const;
    
    // Source code analysis
    struct CodeDocumentation {
        std::string class_name;
        std::string namespace_name;
        std::string description;
        std::vector<std::string> methods;
        std::vector<std::string> properties;
        std::filesystem::path source_file;
    };
    
    std::vector<CodeDocumentation> extractCodeDocumentation(const std::filesystem::path& source_dir) const;
    std::string generateClassDocumentation(const CodeDocumentation& code_doc) const;
    
    // Configuration
    const GitHubPagesConfig& getConfig() const { return config_; }
    void updateConfig(const GitHubPagesConfig& config) { config_ = config; }
    
private:
    GitHubPagesConfig config_;
    std::shared_ptr<MarkdownProcessor> markdown_processor_;
    std::vector<DocumentationPage> pages_;
    std::unordered_map<std::string, std::string> templates_;
    NavigationItem navigation_root_;
    
    bool loadDefaultTemplates();
    bool processMarkdownFiles(const std::filesystem::path& input_dir);
    bool copyAssets(const std::filesystem::path& assets_dir);
    std::string extractClassFromHeader(const std::filesystem::path& header_file) const;
    std::string extractDocComment(const std::string& source_code, const std::string& element_name) const;
};

/**
 * GitHub Pages deployer
 */
class GitHubPagesDeployer {
public:
    GitHubPagesDeployer(const GitHubPagesConfig& config);
    ~GitHubPagesDeployer();
    
    // Deployment operations
    bool deploy();
    bool deployDirectory(const std::filesystem::path& source_dir);
    bool createGitHubPagesRepo();
    bool updateGitHubPagesRepo();
    
    // Git operations
    bool initializeGitRepo(const std::filesystem::path& repo_dir);
    bool commitChanges(const std::string& commit_message);
    bool pushToGitHub();
    bool createBranch(const std::string& branch_name);
    bool switchToBranch(const std::string& branch_name);
    
    // GitHub API operations
    bool enableGitHubPages();
    bool updateGitHubPagesSettings();
    bool checkDeploymentStatus();
    
    // Repository management
    bool cloneRepository(const std::filesystem::path& target_dir);
    bool copyFilesToRepo(const std::filesystem::path& source_dir, const std::filesystem::path& repo_dir);
    
    // Status and logging
    struct DeploymentStatus {
        bool success = false;
        std::string commit_sha;
        std::string deployment_url;
        std::chrono::system_clock::time_point deployment_time;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    const DeploymentStatus& getLastDeploymentStatus() const { return last_deployment_status_; }
    
    // Configuration
    const GitHubPagesConfig& getConfig() const { return config_; }
    void updateConfig(const GitHubPagesConfig& config) { config_ = config; }
    
private:
    GitHubPagesConfig config_;
    DeploymentStatus last_deployment_status_;
    std::filesystem::path temp_repo_dir_;
    
    bool validateConfiguration() const;
    std::string executeGitCommand(const std::string& command, const std::filesystem::path& working_dir) const;
    bool makeHttpRequest(const std::string& method, const std::string& url, 
                        const std::string& data, std::string& response) const;
    std::string buildGitHubApiUrl(const std::string& endpoint) const;
    std::string getAuthorizationHeader() const;
};

/**
 * ElizaOS GitHub.io main interface
 */
class ElizaOSGitHubIO {
public:
    ElizaOSGitHubIO(const GitHubPagesConfig& config = GitHubPagesConfig{});
    ~ElizaOSGitHubIO();
    
    // Initialization
    bool initialize();
    bool isInitialized() const { return initialized_; }
    
    // Documentation workflow
    bool generateAndDeploy();
    bool generateDocumentation();
    bool deployToGitHubPages();
    
    // Component access
    std::shared_ptr<DocumentationGenerator> getDocumentationGenerator() const { return doc_generator_; }
    std::shared_ptr<GitHubPagesDeployer> getDeployer() const { return deployer_; }
    std::shared_ptr<MarkdownProcessor> getMarkdownProcessor() const { return markdown_processor_; }
    
    // Configuration
    const GitHubPagesConfig& getConfig() const { return config_; }
    bool updateConfig(const GitHubPagesConfig& config);
    
    // Status monitoring
    GitHubPagesDeployer::DeploymentStatus getDeploymentStatus() const;
    bool isDeploymentInProgress() const { return deployment_in_progress_; }
    
    // Automation
    bool enableAutoDeployment(const std::string& trigger_branch = "main");
    bool disableAutoDeployment();
    bool watchForChanges(const std::filesystem::path& watch_dir);
    
private:
    GitHubPagesConfig config_;
    std::shared_ptr<DocumentationGenerator> doc_generator_;
    std::shared_ptr<GitHubPagesDeployer> deployer_;
    std::shared_ptr<MarkdownProcessor> markdown_processor_;
    bool initialized_ = false;
    bool deployment_in_progress_ = false;
    bool auto_deployment_enabled_ = false;
    
    bool setupWorkspace();
    bool validateEnvironment();
};

} // namespace elizaos