#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace elizaos {

/**
 * @brief Represents a resource in the awesome eliza ecosystem
 */
struct AwesomeResource {
    std::string name;
    std::string url;
    std::string description;
    std::string category;
    std::string subcategory;
    
    // Default constructor
    AwesomeResource() = default;
    
    AwesomeResource(const std::string& name, const std::string& url, 
                   const std::string& description, const std::string& category,
                   const std::string& subcategory = "")
        : name(name), url(url), description(description), 
          category(category), subcategory(subcategory) {}
};

/**
 * @brief Categories of awesome eliza resources
 */
enum class ResourceCategory {
    OFFICIAL,
    TUTORIALS,
    TOOLS,
    INTEGRATIONS,
    CLIENTS,
    ADAPTERS,
    PLUGINS,
    COMMUNITY,
    VIDEOS,
    RESEARCH
};

/**
 * @brief Subcategories for plugins
 */
enum class PluginCategory {
    BLOCKCHAIN,
    TRADING,
    AI_DATA,
    MEDIA,
    SOCIAL,
    INFRASTRUCTURE,
    UTILITIES
};

/**
 * @brief Manager for awesome eliza resources
 */
class AwesomeElizaManager {
public:
    AwesomeElizaManager();
    ~AwesomeElizaManager() = default;

    // Core functionality
    void initialize();
    std::vector<AwesomeResource> getAllResources() const;
    std::vector<AwesomeResource> getResourcesByCategory(ResourceCategory category) const;
    std::vector<AwesomeResource> getResourcesBySubcategory(const std::string& subcategory) const;
    std::vector<AwesomeResource> searchResources(const std::string& query) const;
    
    // Category-specific getters
    std::vector<AwesomeResource> getOfficialResources() const;
    std::vector<AwesomeResource> getTutorials() const;
    std::vector<AwesomeResource> getTools() const;
    std::vector<AwesomeResource> getClients() const;
    std::vector<AwesomeResource> getAdapters() const;
    std::vector<AwesomeResource> getPlugins() const;
    std::vector<AwesomeResource> getCommunityResources() const;
    std::vector<AwesomeResource> getVideos() const;
    std::vector<AwesomeResource> getResearch() const;
    
    // Plugin-specific functionality
    std::vector<AwesomeResource> getBlockchainPlugins() const;
    std::vector<AwesomeResource> getTradingPlugins() const;
    std::vector<AwesomeResource> getAIPlugins() const;
    std::vector<AwesomeResource> getMediaPlugins() const;
    std::vector<AwesomeResource> getSocialPlugins() const;
    std::vector<AwesomeResource> getInfrastructurePlugins() const;
    std::vector<AwesomeResource> getUtilityPlugins() const;
    
    // Statistics
    size_t getResourceCount() const;
    size_t getResourceCountByCategory(ResourceCategory category) const;
    std::map<std::string, size_t> getCategoryCounts() const;

private:
    std::vector<AwesomeResource> resources_;
    std::map<ResourceCategory, std::vector<size_t>> categoryIndex_;
    std::map<std::string, std::vector<size_t>> subcategoryIndex_;
    
    void loadOfficialResources();
    void loadTutorials();
    void loadTools();
    void loadIntegrations();
    void loadClients();
    void loadAdapters();
    void loadPlugins();
    void loadCommunity();
    void loadVideos();
    void loadResearch();
    
    void addResource(const AwesomeResource& resource);
    void buildIndices();
    std::string categoryToString(ResourceCategory category) const;
};

} // namespace elizaos