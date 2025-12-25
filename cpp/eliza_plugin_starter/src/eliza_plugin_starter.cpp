#include "elizaos/eliza_plugin_starter.hpp"
#include <sstream>
#include <random>
#include <iostream>
#include <algorithm>

namespace elizaos {

// =========================================================================
// PluginVersion Implementation
// =========================================================================

std::string PluginVersion::toString() const {
    std::ostringstream oss;
    oss << major << "." << minor << "." << patch;
    return oss.str();
}

bool PluginVersion::isCompatibleWith(const PluginVersion& other) const {
    // Major version must match for compatibility
    return major == other.major;
}

PluginVersion PluginVersion::parse(const std::string& versionStr) {
    PluginVersion version;
    std::istringstream iss(versionStr);
    char dot;

    if (!(iss >> version.major)) return version;
    if (!(iss >> dot) || dot != '.') return version;
    if (!(iss >> version.minor)) return version;
    if (!(iss >> dot) || dot != '.') return version;
    if (!(iss >> version.patch)) return version;

    return version;
}

// =========================================================================
// PluginConfig Implementation
// =========================================================================

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
    if (it == values_.end()) return defaultValue;

    try {
        return std::any_cast<std::string>(it->second);
    } catch (const std::bad_any_cast&) {
        return defaultValue;
    }
}

int PluginConfig::getInt(const std::string& key, int defaultValue) const {
    auto it = values_.find(key);
    if (it == values_.end()) return defaultValue;

    try {
        return std::any_cast<int>(it->second);
    } catch (const std::bad_any_cast&) {
        return defaultValue;
    }
}

double PluginConfig::getDouble(const std::string& key, double defaultValue) const {
    auto it = values_.find(key);
    if (it == values_.end()) return defaultValue;

    try {
        return std::any_cast<double>(it->second);
    } catch (const std::bad_any_cast&) {
        return defaultValue;
    }
}

bool PluginConfig::getBool(const std::string& key, bool defaultValue) const {
    auto it = values_.find(key);
    if (it == values_.end()) return defaultValue;

    try {
        return std::any_cast<bool>(it->second);
    } catch (const std::bad_any_cast&) {
        return defaultValue;
    }
}

bool PluginConfig::has(const std::string& key) const {
    return values_.find(key) != values_.end();
}

bool PluginConfig::validate(std::string& errorMessage) const {
    for (const auto& key : requiredKeys_) {
        if (!has(key)) {
            errorMessage = "Missing required configuration key: " + key;
            return false;
        }
    }
    return true;
}

void PluginConfig::addRequiredKey(const std::string& key) {
    if (std::find(requiredKeys_.begin(), requiredKeys_.end(), key) == requiredKeys_.end()) {
        requiredKeys_.push_back(key);
    }
}

// =========================================================================
// PluginContext Implementation
// =========================================================================

PluginContext::PluginContext(const std::string& pluginId)
    : pluginId_(pluginId) {
}

void PluginContext::log(const std::string& message) const {
    std::cout << "[" << pluginId_ << "] " << message << std::endl;
}

void PluginContext::logWarning(const std::string& message) const {
    std::cerr << "[" << pluginId_ << "] WARNING: " << message << std::endl;
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

// =========================================================================
// PluginRegistry Implementation
// =========================================================================

PluginRegistry& PluginRegistry::instance() {
    static PluginRegistry registry;
    return registry;
}

void PluginRegistry::registerPlugin(const std::string& pluginId, PluginFactory factory) {
    if (factory) {
        factories_[pluginId] = std::move(factory);
    }
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
    ids.reserve(factories_.size());
    for (const auto& pair : factories_) {
        ids.push_back(pair.first);
    }
    return ids;
}

void PluginRegistry::clear() {
    factories_.clear();
}

// =========================================================================
// PluginManager Implementation
// =========================================================================

PluginManager::PluginManager() = default;

PluginManager::~PluginManager() {
    // Unload all plugins in reverse order
    auto ids = getLoadedPluginIds();
    for (auto it = ids.rbegin(); it != ids.rend(); ++it) {
        unloadPlugin(*it);
    }
}

bool PluginManager::loadPlugin(const std::string& pluginId, const PluginConfig& config) {
    // Check if already loaded
    if (plugins_.find(pluginId) != plugins_.end()) {
        return true;  // Already loaded
    }

    // Create plugin instance from registry
    auto plugin = PluginRegistry::instance().createPlugin(pluginId);
    if (!plugin) {
        std::cerr << "Failed to create plugin: " << pluginId << std::endl;
        return false;
    }

    // Create context for this plugin
    auto context = std::make_unique<PluginContext>(pluginId);
    if (agentState_) {
        context->setAgentState(agentState_);
    }

    // Validate configuration
    std::string configError;
    if (!config.validate(configError)) {
        std::cerr << "Plugin configuration error: " << configError << std::endl;
        return false;
    }

    // Initialize the plugin
    plugin->setState(PluginState::LOADED);
    if (!plugin->initialize(config, *context)) {
        std::cerr << "Failed to initialize plugin: " << pluginId << std::endl;
        plugin->setState(PluginState::ERROR);
        return false;
    }

    plugin->setState(PluginState::INITIALIZED);

    // Check dependencies
    auto metadata = plugin->getMetadata();
    std::vector<std::string> missing;
    if (!PluginUtils::checkDependencies(metadata.dependencies, *this, missing)) {
        std::cerr << "Plugin " << pluginId << " has missing dependencies: ";
        for (const auto& dep : missing) {
            std::cerr << dep << " ";
        }
        std::cerr << std::endl;
        plugin->shutdown();
        return false;
    }

    // Activate the plugin
    plugin->setState(PluginState::ACTIVE);

    // Store plugin and context
    contexts_[pluginId] = std::move(context);
    plugins_[pluginId] = std::move(plugin);

    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginId) {
    auto it = plugins_.find(pluginId);
    if (it == plugins_.end()) {
        return false;
    }

    // Shutdown the plugin
    it->second->shutdown();
    it->second->setState(PluginState::UNLOADED);

    // Remove plugin and context
    plugins_.erase(it);
    contexts_.erase(pluginId);

    return true;
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
    if (!plugin) return false;

    if (plugin->getState() == PluginState::ACTIVE) {
        plugin->setState(PluginState::SUSPENDED);
        return true;
    }
    return false;
}

bool PluginManager::resumePlugin(const std::string& pluginId) {
    auto* plugin = getPlugin(pluginId);
    if (!plugin) return false;

    if (plugin->getState() == PluginState::SUSPENDED) {
        plugin->setState(PluginState::ACTIVE);
        return true;
    }
    return false;
}

std::vector<std::string> PluginManager::getLoadedPluginIds() const {
    std::vector<std::string> ids;
    ids.reserve(plugins_.size());
    for (const auto& pair : plugins_) {
        ids.push_back(pair.first);
    }
    return ids;
}

bool PluginManager::checkPluginHealth(const std::string& pluginId) const {
    auto* plugin = getPlugin(pluginId);
    if (!plugin) return false;
    return plugin->healthCheck();
}

void PluginManager::setAgentState(std::shared_ptr<State> state) {
    agentState_ = state;
    for (auto& pair : contexts_) {
        pair.second->setAgentState(state);
    }
}

// =========================================================================
// SimpleActionPlugin Implementation
// =========================================================================

SimpleActionPlugin::SimpleActionPlugin(const std::string& id, const std::string& name,
                                       const std::string& description) {
    metadata_.id = id;
    metadata_.name = name;
    metadata_.description = description;
    metadata_.version = {1, 0, 0};
}

bool SimpleActionPlugin::initialize(const PluginConfig& /*config*/, PluginContext& context) {
    context_ = &context;
    context_->log("Initializing action plugin: " + metadata_.name);
    return true;
}

void SimpleActionPlugin::shutdown() {
    if (context_) {
        context_->log("Shutting down action plugin: " + metadata_.name);
    }
    context_ = nullptr;
}

PluginMetadata SimpleActionPlugin::getMetadata() const {
    return metadata_;
}

// =========================================================================
// SimpleProviderPlugin Implementation
// =========================================================================

SimpleProviderPlugin::SimpleProviderPlugin(const std::string& id, const std::string& name,
                                           const std::string& description) {
    metadata_.id = id;
    metadata_.name = name;
    metadata_.description = description;
    metadata_.version = {1, 0, 0};
}

bool SimpleProviderPlugin::initialize(const PluginConfig& /*config*/, PluginContext& context) {
    context_ = &context;
    context_->log("Initializing provider plugin: " + metadata_.name);
    return true;
}

void SimpleProviderPlugin::shutdown() {
    if (context_) {
        context_->log("Shutting down provider plugin: " + metadata_.name);
    }
    context_ = nullptr;
}

PluginMetadata SimpleProviderPlugin::getMetadata() const {
    return metadata_;
}

// =========================================================================
// PluginUtils Implementation
// =========================================================================

namespace PluginUtils {

std::string generatePluginId(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);

    std::ostringstream oss;
    oss << prefix << "-" << dis(gen);
    return oss.str();
}

bool validateMetadata(const PluginMetadata& metadata, std::string& error) {
    if (metadata.id.empty()) {
        error = "Plugin ID is required";
        return false;
    }

    if (metadata.name.empty()) {
        error = "Plugin name is required";
        return false;
    }

    // ID should only contain alphanumeric characters and dashes
    for (char c : metadata.id) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            error = "Plugin ID contains invalid characters";
            return false;
        }
    }

    return true;
}

bool checkDependencies(const std::vector<std::string>& deps,
                       const PluginManager& manager,
                       std::vector<std::string>& missing) {
    missing.clear();
    auto loadedPlugins = manager.getLoadedPluginIds();

    for (const auto& dep : deps) {
        if (std::find(loadedPlugins.begin(), loadedPlugins.end(), dep) == loadedPlugins.end()) {
            missing.push_back(dep);
        }
    }

    return missing.empty();
}

std::string stateToString(PluginState state) {
    switch (state) {
        case PluginState::UNLOADED:    return "UNLOADED";
        case PluginState::LOADED:      return "LOADED";
        case PluginState::INITIALIZED: return "INITIALIZED";
        case PluginState::ACTIVE:      return "ACTIVE";
        case PluginState::SUSPENDED:   return "SUSPENDED";
        case PluginState::ERROR:       return "ERROR";
        default:                       return "UNKNOWN";
    }
}

PluginState stateFromString(const std::string& str) {
    if (str == "UNLOADED")    return PluginState::UNLOADED;
    if (str == "LOADED")      return PluginState::LOADED;
    if (str == "INITIALIZED") return PluginState::INITIALIZED;
    if (str == "ACTIVE")      return PluginState::ACTIVE;
    if (str == "SUSPENDED")   return PluginState::SUSPENDED;
    if (str == "ERROR")       return PluginState::ERROR;
    return PluginState::UNLOADED;
}

} // namespace PluginUtils

} // namespace elizaos
