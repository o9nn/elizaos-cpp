// ==============================================================================
// ELIZA PLUGIN STARTER - Simple placeholder implementation
// ==============================================================================

#include "elizaos/eliza_plugin_starter.hpp"
#include <iostream>

namespace elizaos {

// PluginVersion implementation
std::string PluginVersion::toString() const {
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

bool PluginVersion::isCompatibleWith(const PluginVersion& other) const {
    return major == other.major;
}

PluginVersion PluginVersion::parse(const std::string& versionStr) {
    PluginVersion version;
    // Simple parsing - can be enhanced
    sscanf(versionStr.c_str(), "%d.%d.%d", &version.major, &version.minor, &version.patch);
    return version;
}

// PluginConfig implementation
void PluginConfig::set(const std::string& key, const std::string& value) {
    values_[key] = value;
}

void PluginConfig::set(const std::string& key, int value) {
    values_[key] = value;
}

void PluginConfig::set(const std::string& key, double value) {
    values_[key] = value;
}

void PluginConfig::set(const std::string& key, bool value) {
    values_[key] = value;
}

std::string PluginConfig::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        try {
            return std::any_cast<std::string>(it->second);
        } catch (...) {}
    }
    return defaultValue;
}

int PluginConfig::getInt(const std::string& key, int defaultValue) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        try {
            return std::any_cast<int>(it->second);
        } catch (...) {}
    }
    return defaultValue;
}

double PluginConfig::getDouble(const std::string& key, double defaultValue) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        try {
            return std::any_cast<double>(it->second);
        } catch (...) {}
    }
    return defaultValue;
}

bool PluginConfig::getBool(const std::string& key, bool defaultValue) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        try {
            return std::any_cast<bool>(it->second);
        } catch (...) {}
    }
    return defaultValue;
}

bool PluginConfig::has(const std::string& key) const {
    return values_.find(key) != values_.end();
}

bool PluginConfig::validate(std::string& errorMessage) const {
    for (const auto& key : requiredKeys_) {
        if (!has(key)) {
            errorMessage = "Missing required key: " + key;
            return false;
        }
    }
    return true;
}

void PluginConfig::addRequiredKey(const std::string& key) {
    requiredKeys_.push_back(key);
}

// PluginContext implementation
PluginContext::PluginContext(const std::string& pluginId) : pluginId_(pluginId) {}

void PluginContext::log(const std::string& message) const {
    std::cout << "[" << pluginId_ << "] " << message << std::endl;
}

void PluginContext::logWarning(const std::string& message) const {
    std::cout << "[" << pluginId_ << "] WARNING: " << message << std::endl;
}

void PluginContext::logError(const std::string& message) const {
    std::cerr << "[" << pluginId_ << "] ERROR: " << message << std::endl;
}

void PluginContext::setData(const std::string& key, std::any value) {
    pluginData_[key] = std::move(value);
}

std::any PluginContext::getData(const std::string& key) const {
    auto it = pluginData_.find(key);
    return it != pluginData_.end() ? it->second : std::any{};
}

bool PluginContext::hasData(const std::string& key) const {
    return pluginData_.find(key) != pluginData_.end();
}

// PluginRegistry implementation
PluginRegistry& PluginRegistry::instance() {
    static PluginRegistry instance;
    return instance;
}

void PluginRegistry::registerPlugin(const std::string& pluginId, PluginFactory factory) {
    factories_[pluginId] = std::move(factory);
}

void PluginRegistry::unregisterPlugin(const std::string& pluginId) {
    factories_.erase(pluginId);
}

std::unique_ptr<IPlugin> PluginRegistry::createPlugin(const std::string& pluginId) const {
    auto it = factories_.find(pluginId);
    if (it != factories_.end()) {
        return it->second();
    }
    return nullptr;
}

bool PluginRegistry::hasPlugin(const std::string& pluginId) const {
    return factories_.find(pluginId) != factories_.end();
}

std::vector<std::string> PluginRegistry::getRegisteredPluginIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : factories_) {
        ids.push_back(pair.first);
    }
    return ids;
}

void PluginRegistry::clear() {
    factories_.clear();
}

// PluginManager implementation
PluginManager::PluginManager() = default;
PluginManager::~PluginManager() = default;

bool PluginManager::loadPlugin(const std::string& pluginId, const PluginConfig& config) {
    auto plugin = PluginRegistry::instance().createPlugin(pluginId);
    if (!plugin) {
        return false;
    }
    
    auto context = std::make_unique<PluginContext>(pluginId);
    if (agentState_) {
        context->setAgentState(agentState_);
    }
    
    if (plugin->initialize(config, *context)) {
        plugins_[pluginId] = std::move(plugin);
        contexts_[pluginId] = std::move(context);
        return true;
    }
    
    return false;
}

bool PluginManager::unloadPlugin(const std::string& pluginId) {
    auto it = plugins_.find(pluginId);
    if (it != plugins_.end()) {
        it->second->shutdown();
        plugins_.erase(it);
        contexts_.erase(pluginId);
        return true;
    }
    return false;
}

IPlugin* PluginManager::getPlugin(const std::string& pluginId) {
    auto it = plugins_.find(pluginId);
    return it != plugins_.end() ? it->second.get() : nullptr;
}

const IPlugin* PluginManager::getPlugin(const std::string& pluginId) const {
    auto it = plugins_.find(pluginId);
    return it != plugins_.end() ? it->second.get() : nullptr;
}

bool PluginManager::suspendPlugin(const std::string& pluginId) {
    auto* plugin = getPlugin(pluginId);
    if (plugin && plugin->getState() == PluginState::ACTIVE) {
        plugin->setState(PluginState::SUSPENDED);
        return true;
    }
    return false;
}

bool PluginManager::resumePlugin(const std::string& pluginId) {
    auto* plugin = getPlugin(pluginId);
    if (plugin && plugin->getState() == PluginState::SUSPENDED) {
        plugin->setState(PluginState::ACTIVE);
        return true;
    }
    return false;
}

std::vector<std::string> PluginManager::getLoadedPluginIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : plugins_) {
        ids.push_back(pair.first);
    }
    return ids;
}

bool PluginManager::checkPluginHealth(const std::string& pluginId) const {
    auto* plugin = getPlugin(pluginId);
    return plugin && plugin->healthCheck();
}

void PluginManager::setAgentState(std::shared_ptr<State> state) {
    agentState_ = state;
    for (auto& pair : contexts_) {
        pair.second->setAgentState(state);
    }
}

// SimpleActionPlugin implementation
SimpleActionPlugin::SimpleActionPlugin(const std::string& id, const std::string& name,
                                       const std::string& description) {
    metadata_.id = id;
    metadata_.name = name;
    metadata_.description = description;
}

bool SimpleActionPlugin::initialize(const PluginConfig& config, PluginContext& context) {
    context_ = &context;
    state_ = PluginState::INITIALIZED;
    return true;
}

void SimpleActionPlugin::shutdown() {
    state_ = PluginState::UNLOADED;
}

PluginMetadata SimpleActionPlugin::getMetadata() const {
    return metadata_;
}

// SimpleProviderPlugin implementation  
SimpleProviderPlugin::SimpleProviderPlugin(const std::string& id, const std::string& name,
                                           const std::string& description) {
    metadata_.id = id;
    metadata_.name = name;
    metadata_.description = description;
}

bool SimpleProviderPlugin::initialize(const PluginConfig& config, PluginContext& context) {
    context_ = &context;
    state_ = PluginState::INITIALIZED;
    return true;
}

void SimpleProviderPlugin::shutdown() {
    state_ = PluginState::UNLOADED;
}

PluginMetadata SimpleProviderPlugin::getMetadata() const {
    return metadata_;
}

// PluginUtils implementation
namespace PluginUtils {
    std::string generatePluginId(const std::string& prefix) {
        static int counter = 0;
        return prefix + "-" + std::to_string(++counter);
    }

    bool validateMetadata(const PluginMetadata& metadata, std::string& error) {
        if (metadata.id.empty()) {
            error = "Plugin ID cannot be empty";
            return false;
        }
        if (metadata.name.empty()) {
            error = "Plugin name cannot be empty";
            return false;
        }
        return true;
    }

    bool checkDependencies(const std::vector<std::string>& deps,
                          const PluginManager& manager,
                          std::vector<std::string>& missing) {
        missing.clear();
        for (const auto& dep : deps) {
            if (!manager.getPlugin(dep)) {
                missing.push_back(dep);
            }
        }
        return missing.empty();
    }

    std::string stateToString(PluginState state) {
        switch (state) {
            case PluginState::UNLOADED: return "UNLOADED";
            case PluginState::LOADED: return "LOADED";
            case PluginState::INITIALIZED: return "INITIALIZED";
            case PluginState::ACTIVE: return "ACTIVE";
            case PluginState::SUSPENDED: return "SUSPENDED";
            case PluginState::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    PluginState stateFromString(const std::string& str) {
        if (str == "UNLOADED") return PluginState::UNLOADED;
        if (str == "LOADED") return PluginState::LOADED;
        if (str == "INITIALIZED") return PluginState::INITIALIZED;
        if (str == "ACTIVE") return PluginState::ACTIVE;
        if (str == "SUSPENDED") return PluginState::SUSPENDED;
        if (str == "ERROR") return PluginState::ERROR;
        return PluginState::UNLOADED;
    }
}

} // namespace elizaos
