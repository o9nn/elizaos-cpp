#include "elizaos/plugin_specification.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

namespace elizaos {

// PluginMetadata Implementation
PluginMetadata::PluginMetadata(const std::string& name, const PluginAPIVersion& version)
    : name_(name), version_(version) {
}

void PluginMetadata::addCapability(PluginCapability capability) {
    if (std::find(capabilities_.begin(), capabilities_.end(), capability) == capabilities_.end()) {
        capabilities_.push_back(capability);
    }
}

bool PluginMetadata::hasCapability(PluginCapability capability) const {
    return std::find(capabilities_.begin(), capabilities_.end(), capability) != capabilities_.end();
}

void PluginMetadata::addDependency(const PluginDependency& dependency) {
    dependencies_.push_back(dependency);
}

void PluginMetadata::addConfigField(const PluginConfigSchema& field) {
    configSchema_.push_back(field);
}

bool PluginMetadata::isValid() const {
    return validate().empty();
}

std::vector<std::string> PluginMetadata::validate() const {
    std::vector<std::string> errors;
    
    if (name_.empty()) {
        errors.push_back("Plugin name cannot be empty");
    }
    
    if (!plugin_utils::isValidPluginName(name_)) {
        errors.push_back("Plugin name contains invalid characters");
    }
    
    if (capabilities_.empty()) {
        errors.push_back("Plugin must declare at least one capability");
    }
    
    // Validate dependencies
    for (const auto& dep : dependencies_) {
        if (dep.name.empty()) {
            errors.push_back("Dependency name cannot be empty");
        }
        if (!dep.minVersion.isCompatible(dep.maxVersion)) {
            errors.push_back("Invalid dependency version range for " + dep.name);
        }
    }
    
    return errors;
}

// PluginManager Implementation
PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
    // Unload all plugins in reverse dependency order
    for (auto& [name, plugin] : loadedPlugins_) {
        plugin->stop();
        plugin->shutdown();
    }
    loadedPlugins_.clear();
}

std::vector<PluginInfo> PluginManager::discoverPlugins(const std::string& directory) {
    std::vector<PluginInfo> plugins;
    
    try {
        if (!std::filesystem::exists(directory)) {
            return plugins;
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                const std::string path = entry.path().string();
                
                // Look for plugin manifest files or shared libraries
                if ((path.size() >= 12 && path.substr(path.size() - 12) == ".plugin.json") ||
                    (path.size() >= 3 && path.substr(path.size() - 3) == ".so") ||
                    (path.size() >= 4 && path.substr(path.size() - 4) == ".dll")) {
                    if (validatePluginPath(path)) {
                        PluginInfo info;
                        info.name = entry.path().stem().string();
                        info.path = path;
                        info.metadata = loadPluginMetadata(path);
                        info.isLoaded = loadedPlugins_.find(info.name) != loadedPlugins_.end();
                        
                        plugins.push_back(info);
                        availablePlugins_[info.name] = info;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // Log error but continue
        notifyEvent("system", "Plugin discovery error: " + std::string(e.what()));
    }
    
    return plugins;
}

std::vector<PluginInfo> PluginManager::getAvailablePlugins() const {
    std::vector<PluginInfo> plugins;
    for (const auto& [name, info] : availablePlugins_) {
        plugins.push_back(info);
    }
    return plugins;
}

PluginLoadResult PluginManager::loadPlugin(const std::string& path, 
                                          const std::unordered_map<std::string, std::string>& config) {
    try {
        // Extract plugin name from path
        std::filesystem::path p(path);
        std::string name = p.stem().string();
        
        // Check if already loaded
        if (loadedPlugins_.find(name) != loadedPlugins_.end()) {
            return PluginLoadResult(false, "Plugin already loaded");
        }
        
        // Load metadata
        PluginMetadata metadata = loadPluginMetadata(path);
        
        // Validate dependencies
        if (!validateDependencies(metadata)) {
            return PluginLoadResult(false, "Dependency validation failed");
        }
        
        // Try to create plugin from registry
        auto plugin = PluginRegistry::getInstance().createPlugin(name);
        if (!plugin) {
            return PluginLoadResult(false, "Plugin factory not found");
        }
        
        // Initialize plugin
        if (!plugin->initialize(config)) {
            return PluginLoadResult(false, "Plugin initialization failed");
        }
        
        // Store plugin
        loadedPlugins_[name] = plugin;
        pluginConfigs_[name] = config;
        
        // Update available plugins info
        if (availablePlugins_.find(name) != availablePlugins_.end()) {
            availablePlugins_[name].isLoaded = true;
            availablePlugins_[name].status = "loaded";
        }
        
        notifyEvent(name, "loaded");
        
        PluginLoadResult result(true);
        result.plugin = plugin;
        result.metadata = metadata;
        return result;
        
    } catch (const std::exception& e) {
        return PluginLoadResult(false, e.what());
    }
}

bool PluginManager::unloadPlugin(const std::string& name) {
    auto it = loadedPlugins_.find(name);
    if (it == loadedPlugins_.end()) {
        return false;
    }
    
    try {
        it->second->stop();
        it->second->shutdown();
        loadedPlugins_.erase(it);
        pluginConfigs_.erase(name);
        
        // Update available plugins info
        if (availablePlugins_.find(name) != availablePlugins_.end()) {
            availablePlugins_[name].isLoaded = false;
            availablePlugins_[name].status = "unloaded";
        }
        
        notifyEvent(name, "unloaded");
        return true;
        
    } catch (const std::exception& e) {
        notifyEvent(name, "unload_error: " + std::string(e.what()));
        return false;
    }
}

bool PluginManager::enablePlugin(const std::string& name) {
    auto it = loadedPlugins_.find(name);
    if (it == loadedPlugins_.end()) {
        return false;
    }
    
    try {
        bool result = it->second->start();
        if (result && availablePlugins_.find(name) != availablePlugins_.end()) {
            availablePlugins_[name].isEnabled = true;
            availablePlugins_[name].status = "enabled";
        }
        
        notifyEvent(name, result ? "enabled" : "enable_failed");
        return result;
        
    } catch (const std::exception& e) {
        notifyEvent(name, "enable_error: " + std::string(e.what()));
        return false;
    }
}

bool PluginManager::disablePlugin(const std::string& name) {
    auto it = loadedPlugins_.find(name);
    if (it == loadedPlugins_.end()) {
        return false;
    }
    
    try {
        bool result = it->second->stop();
        if (result && availablePlugins_.find(name) != availablePlugins_.end()) {
            availablePlugins_[name].isEnabled = false;
            availablePlugins_[name].status = "disabled";
        }
        
        notifyEvent(name, result ? "disabled" : "disable_failed");
        return result;
        
    } catch (const std::exception& e) {
        notifyEvent(name, "disable_error: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<PluginInterface> PluginManager::getPlugin(const std::string& name) const {
    auto it = loadedPlugins_.find(name);
    return (it != loadedPlugins_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<PluginInterface>> PluginManager::getPluginsByCapability(PluginCapability capability) const {
    std::vector<std::shared_ptr<PluginInterface>> results;
    
    for (const auto& [name, plugin] : loadedPlugins_) {
        if (plugin->getMetadata().hasCapability(capability)) {
            results.push_back(plugin);
        }
    }
    
    return results;
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const {
    std::vector<std::string> names;
    for (const auto& [name, plugin] : loadedPlugins_) {
        names.push_back(name);
    }
    return names;
}

bool PluginManager::configurePlugin(const std::string& name, 
                                   const std::unordered_map<std::string, std::string>& config) {
    auto it = loadedPlugins_.find(name);
    if (it == loadedPlugins_.end()) {
        return false;
    }
    
    if (it->second->validateConfig(config)) {
        pluginConfigs_[name] = config;
        return true;
    }
    
    return false;
}

std::unordered_map<std::string, std::string> PluginManager::getPluginConfig(const std::string& name) const {
    auto it = pluginConfigs_.find(name);
    return (it != pluginConfigs_.end()) ? it->second : std::unordered_map<std::string, std::string>{};
}

bool PluginManager::validateDependencies(const PluginMetadata& metadata) const {
    for (const auto& dep : metadata.getDependencies()) {
        if (!dep.optional) {
            auto it = loadedPlugins_.find(dep.name);
            if (it == loadedPlugins_.end()) {
                return false;
            }
            
            const auto& depMetadata = it->second->getMetadata();
            if (!dep.minVersion.isCompatible(depMetadata.getVersion())) {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> PluginManager::resolveDependencyOrder(const std::vector<std::string>& pluginNames) const {
    // Simple topological sort for dependency resolution
    std::vector<std::string> sorted;
    std::vector<std::string> remaining = pluginNames;
    
    while (!remaining.empty()) {
        size_t initialSize = remaining.size();
        
        for (auto it = remaining.begin(); it != remaining.end();) {
            const std::string& name = *it;
            
            // Check if all dependencies are already sorted
            bool canAdd = true;
            auto pluginIt = availablePlugins_.find(name);
            if (pluginIt != availablePlugins_.end()) {
                for (const auto& dep : pluginIt->second.metadata.getDependencies()) {
                    if (!dep.optional && 
                        std::find(sorted.begin(), sorted.end(), dep.name) == sorted.end()) {
                        canAdd = false;
                        break;
                    }
                }
            }
            
            if (canAdd) {
                sorted.push_back(name);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }
        
        // Check for circular dependencies
        if (remaining.size() == initialSize) {
            // Circular dependency detected, add remaining plugins anyway
            for (const auto& name : remaining) {
                sorted.push_back(name);
            }
            break;
        }
    }
    
    return sorted;
}

std::vector<std::string> PluginManager::getUnhealthyPlugins() const {
    std::vector<std::string> unhealthy;
    
    for (const auto& [name, plugin] : loadedPlugins_) {
        if (!plugin->isHealthy()) {
            unhealthy.push_back(name);
        }
    }
    
    return unhealthy;
}

bool PluginManager::performHealthCheck(const std::string& name) const {
    auto it = loadedPlugins_.find(name);
    return (it != loadedPlugins_.end()) ? it->second->isHealthy() : false;
}

void PluginManager::performAllHealthChecks() {
    for (const auto& [name, plugin] : loadedPlugins_) {
        bool healthy = plugin->isHealthy();
        notifyEvent(name, healthy ? "health_ok" : "health_failed");
    }
}

bool PluginManager::validatePluginPath(const std::string& path) const {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

PluginMetadata PluginManager::loadPluginMetadata(const std::string& path) const {
    // For now, return basic metadata - in a real implementation,
    // this would parse a manifest file or extract metadata from the binary
    std::filesystem::path p(path);
    std::string name = p.stem().string();
    
    PluginMetadata metadata(name, PluginAPIVersion(1, 0, 0));
    metadata.setDescription("Plugin loaded from " + path);
    metadata.addCapability(PluginCapability::CUSTOM);
    
    return metadata;
}

void PluginManager::notifyEvent(const std::string& pluginName, const std::string& event) {
    if (eventCallback_) {
        eventCallback_(pluginName, event);
    }
}

// PluginRegistry Implementation
PluginRegistry& PluginRegistry::getInstance() {
    static PluginRegistry instance;
    return instance;
}

void PluginRegistry::registerPlugin(const std::string& name, PluginFactory factory, const PluginMetadata& metadata) {
    factories_[name] = factory;
    metadata_[name] = metadata;
}

void PluginRegistry::unregisterPlugin(const std::string& name) {
    factories_.erase(name);
    metadata_.erase(name);
}

std::shared_ptr<PluginInterface> PluginRegistry::createPlugin(const std::string& name) const {
    auto it = factories_.find(name);
    return (it != factories_.end()) ? it->second() : nullptr;
}

std::vector<std::string> PluginRegistry::getRegisteredPlugins() const {
    std::vector<std::string> names;
    for (const auto& [name, factory] : factories_) {
        names.push_back(name);
    }
    return names;
}

PluginMetadata PluginRegistry::getPluginMetadata(const std::string& name) const {
    auto it = metadata_.find(name);
    if (it != metadata_.end()) {
        return it->second;
    }
    return PluginMetadata("unknown", PluginAPIVersion());
}

// Utility functions
namespace plugin_utils {

std::string capabilityToString(PluginCapability capability) {
    switch (capability) {
        case PluginCapability::ACTION_PROVIDER: return "action_provider";
        case PluginCapability::MEMORY_PROVIDER: return "memory_provider";
        case PluginCapability::EVALUATOR: return "evaluator";
        case PluginCapability::PROVIDER: return "provider";
        case PluginCapability::CHARACTER_PROVIDER: return "character_provider";
        case PluginCapability::KNOWLEDGE_PROVIDER: return "knowledge_provider";
        case PluginCapability::COMMUNICATION: return "communication";
        case PluginCapability::TOOL_INTEGRATION: return "tool_integration";
        case PluginCapability::CUSTOM: return "custom";
        default: return "unknown";
    }
}

PluginCapability stringToCapability(const std::string& str) {
    if (str == "action_provider") return PluginCapability::ACTION_PROVIDER;
    if (str == "memory_provider") return PluginCapability::MEMORY_PROVIDER;
    if (str == "evaluator") return PluginCapability::EVALUATOR;
    if (str == "provider") return PluginCapability::PROVIDER;
    if (str == "character_provider") return PluginCapability::CHARACTER_PROVIDER;
    if (str == "knowledge_provider") return PluginCapability::KNOWLEDGE_PROVIDER;
    if (str == "communication") return PluginCapability::COMMUNICATION;
    if (str == "tool_integration") return PluginCapability::TOOL_INTEGRATION;
    if (str == "custom") return PluginCapability::CUSTOM;
    return PluginCapability::CUSTOM;
}

bool isValidPluginName(const std::string& name) {
    if (name.empty() || name.length() > 64) {
        return false;
    }
    
    // Plugin names should be alphanumeric with hyphens and underscores
    std::regex namePattern("^[a-zA-Z0-9_-]+$");
    return std::regex_match(name, namePattern);
}

bool isValidVersion(const std::string& version) {
    std::regex versionPattern("^\\d+\\.\\d+\\.\\d+$");
    return std::regex_match(version, versionPattern);
}

std::string generatePluginID(const PluginMetadata& metadata) {
    return metadata.getName() + "-" + metadata.getVersion().toString();
}

std::vector<std::string> parsePluginPath(const std::string& path) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string item;
    
    while (std::getline(ss, item, '/')) {
        if (!item.empty()) {
            parts.push_back(item);
        }
    }
    
    return parts;
}

} // namespace plugin_utils

} // namespace elizaos
