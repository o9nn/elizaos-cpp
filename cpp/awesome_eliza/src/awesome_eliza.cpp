#include "elizaos/awesome_eliza.hpp"
#include <algorithm>
#include <sstream>

namespace elizaos {

AwesomeElizaManager::AwesomeElizaManager() {
    initialize();
}

void AwesomeElizaManager::initialize() {
    resources_.clear();
    categoryIndex_.clear();
    subcategoryIndex_.clear();
    
    loadOfficialResources();
    loadTutorials();
    loadTools();
    loadIntegrations();
    loadClients();
    loadAdapters();
    loadPlugins();
    loadCommunity();
    loadVideos();
    loadResearch();
    
    buildIndices();
}

void AwesomeElizaManager::loadOfficialResources() {
    addResource(AwesomeResource("elizaOS GitHub Repository", 
                               "https://github.com/elizaos/eliza",
                               "The main repository for elizaOS, an open-source AI agent framework",
                               "official"));
    
    addResource(AwesomeResource("elizaOS Documentation",
                               "https://elizaos.github.io/eliza/",
                               "Official documentation and guides",
                               "official"));
    
    addResource(AwesomeResource("elizaOS How",
                               "https://eliza.how/",
                               "Ask anything about Eliza",
                               "official"));
    
    addResource(AwesomeResource("Sample Characters",
                               "https://github.com/elizaos/characters",
                               "Sample character templates and implementations", 
                               "official"));
    
    addResource(AwesomeResource("elizaOS Plugin Registry",
                               "https://github.com/elizaos-plugins/registry",
                               "JSON Registry for all the plugins in the elizaOS ecosystem",
                               "official"));
}

void AwesomeElizaManager::loadTutorials() {
    addResource(AwesomeResource("Quickstart Guide",
                               "https://elizaos.github.io/eliza/docs/quickstart",
                               "Get up and running quickly with elizaOS",
                               "tutorials"));
    
    addResource(AwesomeResource("Building a Social AI Agent in 15 Minutes",
                               "https://www.youtube.com/embed/6PZVwNTl5hI",
                               "Nader's Tutorial",
                               "tutorials"));
    
    addResource(AwesomeResource("Creating an AI Agent with Your Own Personality",
                               "https://www.youtube.com/embed/uouSdtcWXTQ",
                               "Nader's Tutorial",
                               "tutorials"));
    
    addResource(AwesomeResource("How to Build an API Plugin",
                               "https://www.youtube.com/embed/25FxjscBHuo",
                               "Nader's Tutorial with code examples",
                               "tutorials"));
}

void AwesomeElizaManager::loadTools() {
    addResource(AwesomeResource("elizagen",
                               "https://elizagen.howieduhzit.best/",
                               "Easy eliza characterfile generator, add your own API keys",
                               "tools"));
    
    addResource(AwesomeResource("Fleek.xyz",
                               "https://fleek.xyz/eliza/",
                               "Deploy Eliza agents in one click on a hosted platform",
                               "tools"));
    
    addResource(AwesomeResource("Xnomad mcv",
                               "https://github.com/xNomad-AI/mcv",
                               "An out-of-box dev kit to launch AI-NFT based on Eliza's character files",
                               "tools"));
}

void AwesomeElizaManager::loadIntegrations() {
    addResource(AwesomeResource("Hyperfy",
                               "https://hyperfy.how/guides/ai/agents/",
                               "agent creation, autonomous behavior, NPC development in open source 3d web",
                               "integrations"));
    
    addResource(AwesomeResource("Nifty Island",
                               "https://docs.niftyisland.com/creator/agents/tutorials/eliza/",
                               "Deploy elizas into Nifty Island and beyond",
                               "integrations"));
}

void AwesomeElizaManager::loadClients() {
    addResource(AwesomeResource("Discord",
                               "https://github.com/elizaos-plugins/client-discord",
                               "Discord integration with messaging and community interaction capabilities",
                               "clients"));
    
    addResource(AwesomeResource("Telegram",
                               "https://github.com/elizaos-plugins/client-telegram",
                               "Telegram messaging client with bot and channel integration capabilities",
                               "clients"));
    
    addResource(AwesomeResource("Slack",
                               "https://github.com/elizaos-plugins/client-slack",
                               "Slack integration with messaging and workspace collaboration features",
                               "clients"));
    
    addResource(AwesomeResource("GitHub",
                               "https://github.com/elizaos-plugins/client-github",
                               "GitHub client for repository management and code integration",
                               "clients"));
    
    addResource(AwesomeResource("Farcaster",
                               "https://github.com/elizaos-plugins/client-farcaster",
                               "Client for Farcaster decentralized social media protocol",
                               "clients"));
}

void AwesomeElizaManager::loadAdapters() {
    addResource(AwesomeResource("MongoDB",
                               "https://github.com/elizaos-plugins/adapter-mongodb",
                               "Database adapter for MongoDB enabling document storage with query capabilities",
                               "adapters"));
    
    addResource(AwesomeResource("PostgreSQL",
                               "https://github.com/elizaos-plugins/adapter-postgres",
                               "Database adapter for PostgreSQL with full relational database capabilities",
                               "adapters"));
    
    addResource(AwesomeResource("SQLite",
                               "https://github.com/elizaos-plugins/adapter-sqlite",
                               "File-based SQL database adapter for lightweight local storage",
                               "adapters"));
    
    addResource(AwesomeResource("Supabase",
                               "https://github.com/elizaos-plugins/adapter-supabase",
                               "Database adapter for Supabase with PostgreSQL and realtime capabilities",
                               "adapters"));
}

void AwesomeElizaManager::loadPlugins() {
    // Blockchain plugins
    addResource(AwesomeResource("Solana",
                               "https://github.com/elizaos-plugins/plugin-solana",
                               "Comprehensive DeFi operations on the Solana blockchain",
                               "plugins", "blockchain"));
    
    addResource(AwesomeResource("EVM",
                               "https://github.com/elizaos-plugins/plugin-evm",
                               "Comprehensive functionality for interacting with EVM-compatible chains",
                               "plugins", "blockchain"));
    
    addResource(AwesomeResource("NEAR",
                               "https://github.com/elizaos-plugins/plugin-near",
                               "Token management and transfers on the NEAR Protocol blockchain",
                               "plugins", "blockchain"));
    
    // Trading plugins
    addResource(AwesomeResource("Binance",
                               "https://github.com/elizaos-plugins/plugin-binance",
                               "Cryptocurrency trading and price checking via Binance API",
                               "plugins", "trading"));
    
    addResource(AwesomeResource("CoinGecko",
                               "https://github.com/elizaos-plugins/plugin-coingecko",
                               "Cryptocurrency price data and market information from CoinGecko",
                               "plugins", "trading"));
    
    // AI & Data plugins
    addResource(AwesomeResource("OpenAI",
                               "https://github.com/elizaos-plugins/plugin-openai",
                               "Integration with OpenAI's GPT models for automated text generation",
                               "plugins", "ai"));
    
    addResource(AwesomeResource("Image Generation",
                               "https://github.com/elizaos-plugins/plugin-image-generation",
                               "Generates and manages images with storage and optimization features",
                               "plugins", "media"));
    
    // Social plugins
    addResource(AwesomeResource("Email",
                               "https://github.com/elizaos-plugins/plugin-email",
                               "Email client functionality for sending/receiving via SMTP/IMAP",
                               "plugins", "social"));
    
    // Infrastructure plugins
    addResource(AwesomeResource("AWS S3",
                               "https://github.com/elizaos-plugins/plugin-aws-s3",
                               "AWS S3 integration for cloud-based file storage management",
                               "plugins", "infrastructure"));
    
    // Utility plugins
    addResource(AwesomeResource("Browser",
                               "https://github.com/elizaos-plugins/plugin-browser",
                               "Web scraping and browser automation using Playwright",
                               "plugins", "utilities"));
}

void AwesomeElizaManager::loadCommunity() {
    addResource(AwesomeResource("elizaOS Homepage",
                               "https://elizaos.ai",
                               "Official homepage for elizaOS",
                               "community"));
    
    addResource(AwesomeResource("elizaOS Twitter",
                               "https://x.com/elizaos",
                               "Official X account for elizaOS",
                               "community"));
    
    addResource(AwesomeResource("awesome-eliza",
                               "https://awesome.eliza.fyi",
                               "HTML version of awesome things related to elizaOS/eliza framework",
                               "community"));
}

void AwesomeElizaManager::loadVideos() {
    addResource(AwesomeResource("The Delphi Podcast: Crypto x AI Agents",
                               "https://www.youtube.com/watch?v=HVXxprDVMUM",
                               "Panel with ai16z, Virtuals, MyShell, NOUS, and CENTS",
                               "videos"));
    
    addResource(AwesomeResource("Bankless + Shaw Interview",
                               "https://www.youtube.com/watch?v=5GBXS5myXz0",
                               "What is the role of AI in crypto, and how will it shape the future?",
                               "videos"));
}

void AwesomeElizaManager::loadResearch() {
    addResource(AwesomeResource("Eliza Technical Report/Whitepaper",
                               "https://arxiv.org/pdf/2501.06781",
                               "Academic paper on ElizaOS architecture and capabilities",
                               "research"));
    
    addResource(AwesomeResource("The Battle of the AI Agent Frameworks",
                               "https://members.delphidigital.io/reports/the-battle-of-the-ai-agent-frameworks",
                               "Delphi research report",
                               "research"));
}

void AwesomeElizaManager::addResource(const AwesomeResource& resource) {
    resources_.push_back(resource);
}

void AwesomeElizaManager::buildIndices() {
    categoryIndex_.clear();
    subcategoryIndex_.clear();
    
    for (size_t i = 0; i < resources_.size(); ++i) {
        const auto& resource = resources_[i];
        
        // Build category index
        ResourceCategory cat;
        if (resource.category == "official") cat = ResourceCategory::OFFICIAL;
        else if (resource.category == "tutorials") cat = ResourceCategory::TUTORIALS;
        else if (resource.category == "tools") cat = ResourceCategory::TOOLS;
        else if (resource.category == "integrations") cat = ResourceCategory::INTEGRATIONS;
        else if (resource.category == "clients") cat = ResourceCategory::CLIENTS;
        else if (resource.category == "adapters") cat = ResourceCategory::ADAPTERS;
        else if (resource.category == "plugins") cat = ResourceCategory::PLUGINS;
        else if (resource.category == "community") cat = ResourceCategory::COMMUNITY;
        else if (resource.category == "videos") cat = ResourceCategory::VIDEOS;
        else if (resource.category == "research") cat = ResourceCategory::RESEARCH;
        else continue;
        
        categoryIndex_[cat].push_back(i);
        
        // Build subcategory index
        if (!resource.subcategory.empty()) {
            subcategoryIndex_[resource.subcategory].push_back(i);
        }
    }
}

std::vector<AwesomeResource> AwesomeElizaManager::getAllResources() const {
    return resources_;
}

std::vector<AwesomeResource> AwesomeElizaManager::getResourcesByCategory(ResourceCategory category) const {
    std::vector<AwesomeResource> result;
    auto it = categoryIndex_.find(category);
    if (it != categoryIndex_.end()) {
        for (size_t idx : it->second) {
            result.push_back(resources_[idx]);
        }
    }
    return result;
}

std::vector<AwesomeResource> AwesomeElizaManager::getResourcesBySubcategory(const std::string& subcategory) const {
    std::vector<AwesomeResource> result;
    auto it = subcategoryIndex_.find(subcategory);
    if (it != subcategoryIndex_.end()) {
        for (size_t idx : it->second) {
            result.push_back(resources_[idx]);
        }
    }
    return result;
}

std::vector<AwesomeResource> AwesomeElizaManager::searchResources(const std::string& query) const {
    std::vector<AwesomeResource> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    
    for (const auto& resource : resources_) {
        std::string lowerName = resource.name;
        std::string lowerDesc = resource.description;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        
        if (lowerName.find(lowerQuery) != std::string::npos ||
            lowerDesc.find(lowerQuery) != std::string::npos) {
            result.push_back(resource);
        }
    }
    return result;
}

std::vector<AwesomeResource> AwesomeElizaManager::getOfficialResources() const {
    return getResourcesByCategory(ResourceCategory::OFFICIAL);
}

std::vector<AwesomeResource> AwesomeElizaManager::getTutorials() const {
    return getResourcesByCategory(ResourceCategory::TUTORIALS);
}

std::vector<AwesomeResource> AwesomeElizaManager::getTools() const {
    return getResourcesByCategory(ResourceCategory::TOOLS);
}

std::vector<AwesomeResource> AwesomeElizaManager::getClients() const {
    return getResourcesByCategory(ResourceCategory::CLIENTS);
}

std::vector<AwesomeResource> AwesomeElizaManager::getAdapters() const {
    return getResourcesByCategory(ResourceCategory::ADAPTERS);
}

std::vector<AwesomeResource> AwesomeElizaManager::getPlugins() const {
    return getResourcesByCategory(ResourceCategory::PLUGINS);
}

std::vector<AwesomeResource> AwesomeElizaManager::getCommunityResources() const {
    return getResourcesByCategory(ResourceCategory::COMMUNITY);
}

std::vector<AwesomeResource> AwesomeElizaManager::getVideos() const {
    return getResourcesByCategory(ResourceCategory::VIDEOS);
}

std::vector<AwesomeResource> AwesomeElizaManager::getResearch() const {
    return getResourcesByCategory(ResourceCategory::RESEARCH);
}

std::vector<AwesomeResource> AwesomeElizaManager::getBlockchainPlugins() const {
    return getResourcesBySubcategory("blockchain");
}

std::vector<AwesomeResource> AwesomeElizaManager::getTradingPlugins() const {
    return getResourcesBySubcategory("trading");
}

std::vector<AwesomeResource> AwesomeElizaManager::getAIPlugins() const {
    return getResourcesBySubcategory("ai");
}

std::vector<AwesomeResource> AwesomeElizaManager::getMediaPlugins() const {
    return getResourcesBySubcategory("media");
}

std::vector<AwesomeResource> AwesomeElizaManager::getSocialPlugins() const {
    return getResourcesBySubcategory("social");
}

std::vector<AwesomeResource> AwesomeElizaManager::getInfrastructurePlugins() const {
    return getResourcesBySubcategory("infrastructure");
}

std::vector<AwesomeResource> AwesomeElizaManager::getUtilityPlugins() const {
    return getResourcesBySubcategory("utilities");
}

size_t AwesomeElizaManager::getResourceCount() const {
    return resources_.size();
}

size_t AwesomeElizaManager::getResourceCountByCategory(ResourceCategory category) const {
    auto it = categoryIndex_.find(category);
    return (it != categoryIndex_.end()) ? it->second.size() : 0;
}

std::map<std::string, size_t> AwesomeElizaManager::getCategoryCounts() const {
    std::map<std::string, size_t> counts;
    
    counts["official"] = getResourceCountByCategory(ResourceCategory::OFFICIAL);
    counts["tutorials"] = getResourceCountByCategory(ResourceCategory::TUTORIALS);
    counts["tools"] = getResourceCountByCategory(ResourceCategory::TOOLS);
    counts["integrations"] = getResourceCountByCategory(ResourceCategory::INTEGRATIONS);
    counts["clients"] = getResourceCountByCategory(ResourceCategory::CLIENTS);
    counts["adapters"] = getResourceCountByCategory(ResourceCategory::ADAPTERS);
    counts["plugins"] = getResourceCountByCategory(ResourceCategory::PLUGINS);
    counts["community"] = getResourceCountByCategory(ResourceCategory::COMMUNITY);
    counts["videos"] = getResourceCountByCategory(ResourceCategory::VIDEOS);
    counts["research"] = getResourceCountByCategory(ResourceCategory::RESEARCH);
    
    return counts;
}

std::string AwesomeElizaManager::categoryToString(ResourceCategory category) const {
    switch (category) {
        case ResourceCategory::OFFICIAL: return "official";
        case ResourceCategory::TUTORIALS: return "tutorials";
        case ResourceCategory::TOOLS: return "tools";
        case ResourceCategory::INTEGRATIONS: return "integrations";
        case ResourceCategory::CLIENTS: return "clients";
        case ResourceCategory::ADAPTERS: return "adapters";
        case ResourceCategory::PLUGINS: return "plugins";
        case ResourceCategory::COMMUNITY: return "community";
        case ResourceCategory::VIDEOS: return "videos";
        case ResourceCategory::RESEARCH: return "research";
        default: return "unknown";
    }
}

} // namespace elizaos
