#include "elizaos/plugin_specification.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include <limits>
#include <stdexcept>
#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
// Re-suppress ERROR macro from windows.h
#ifdef ERROR
#undef ERROR
#endif
#endif

namespace elizaos {

// Global plugin manager instance
std::shared_ptr<PluginManager> globalPluginManager = std::make_shared<PluginManager>();

// =====================================================
// PluginVersion Implementation
// =====================================================

std::string PluginVersion::toString() const {
    std::ostringstream oss;
    oss << major << "." << minor << "." << patch;
    if (!prerelease.empty()) oss << "-" << prerelease;
    if (!build.empty()) oss << "+" << build;
    return oss.str();
}

namespace {

int compareVersions(const PluginVersion& lhs, const PluginVersion& rhs) {
    if (lhs.major != rhs.major) return lhs.major < rhs.major ? -1 : 1;
    if (lhs.minor != rhs.minor) return lhs.minor < rhs.minor ? -1 : 1;
    if (lhs.patch != rhs.patch) return lhs.patch < rhs.patch ? -1 : 1;
    if (lhs.prerelease == rhs.prerelease) return 0;
    if (lhs.prerelease.empty()) return 1;
    if (rhs.prerelease.empty()) return -1;
    return lhs.prerelease < rhs.prerelease ? -1 : 1;
}

bool validIdentifier(const std::string& value) {
    if (value.empty()) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.';
    });
}

bool validVersion(const PluginVersion& version) {
    return version.major >= 0 && version.minor >= 0 && version.patch >= 0 &&
           (version.major != 0 || version.minor != 0 || version.patch != 0);
}

} // namespace

bool PluginVersion::isCompatibleWith(const PluginVersion& other) const {
    return major == other.major && compareVersions(*this, other) >= 0;
}

PluginVersion PluginVersion::fromString(const std::string& versionStr) {
    PluginVersion invalid{0, 0, 0};
    if (versionStr.empty()) return invalid;

    PluginVersion version{0, 0, 0};
    std::string working = versionStr;
    const auto buildPos = working.find('+');
    if (buildPos != std::string::npos) {
        version.build = working.substr(buildPos + 1);
        working = working.substr(0, buildPos);
        if (version.build.empty()) return invalid;
    }
    const auto prereleasePos = working.find('-');
    if (prereleasePos != std::string::npos) {
        version.prerelease = working.substr(prereleasePos + 1);
        working = working.substr(0, prereleasePos);
        if (version.prerelease.empty()) return invalid;
    }

    std::istringstream stream(working);
    std::string part;
    int* values[] = {&version.major, &version.minor, &version.patch};
    int count = 0;
    while (std::getline(stream, part, '.')) {
        if (count == 3 || part.empty() ||
            !std::all_of(part.begin(), part.end(), [](unsigned char ch) {
                return ch >= '0' && ch <= '9';
            })) return invalid;
        try {
            std::size_t consumed = 0;
            const long parsed = std::stol(part, &consumed);
            if (consumed != part.size() || parsed > std::numeric_limits<int>::max()) return invalid;
            *values[count++] = static_cast<int>(parsed);
        } catch (...) {
            return invalid;
        }
    }
    return count == 3 ? version : invalid;
}

// =====================================================
// PluginDependency Implementation
// =====================================================

bool PluginDependency::isSatisfiedBy(const PluginVersion& version) const {
    if (!validVersion(version) || !validVersion(minVersion) ||
        compareVersions(version, minVersion) < 0) return false;
    const bool unbounded = maxVersion.major == 0 && maxVersion.minor == 0 &&
                           maxVersion.patch == 0;
    return unbounded || compareVersions(version, maxVersion) <= 0;
}

// =====================================================
// PluginParameter Implementation
// =====================================================

JsonValue PluginParameter::toJson() const {
    JsonValue json;
    json["name"] = std::string(name);
    json["description"] = std::string(description);
    json["type"] = std::string(type);
    json["required"] = std::string(required ? "true" : "false");
    
    // Serialize default value based on type
    if (type == "string") {
        try {
            json["defaultValue"] = std::string(std::any_cast<std::string>(defaultValue));
        } catch (const std::bad_any_cast&) {
            json["defaultValue"] = std::string("");
        }
    } else if (type == "int") {
        try {
            json["defaultValue"] = std::string(std::to_string(std::any_cast<int>(defaultValue)));
        } catch (const std::bad_any_cast&) {
            json["defaultValue"] = std::string("0");
        }
    } else if (type == "bool") {
        try {
            json["defaultValue"] = std::string(std::any_cast<bool>(defaultValue) ? "true" : "false");
        } catch (const std::bad_any_cast&) {
            json["defaultValue"] = std::string("false");
        }
    }
    
    return json;
}

PluginParameter PluginParameter::fromJson(const JsonValue& json) {
    PluginParameter param;
    
    auto getString = [&](const std::string& key) -> std::string {
        auto it = json.find(key);
        if (it != json.end()) {
            try {
                return std::any_cast<std::string>(it->second);
            } catch (const std::bad_any_cast&) {
                return "";
            }
        }
        return "";
    };
    
    param.name = getString("name");
    param.description = getString("description");
    param.type = getString("type");
    param.required = getString("required") == "true";
    
    // Parse default value based on type
    std::string defaultStr = getString("defaultValue");
    if (param.type == "string") {
        param.defaultValue = defaultStr;
    } else if (param.type == "int") {
        param.defaultValue = std::stoi(defaultStr);
    } else if (param.type == "bool") {
        param.defaultValue = defaultStr == "true";
    }
    
    return param;
}

// =====================================================
// PluginMetadata Implementation
// =====================================================

JsonValue PluginMetadata::toJson() const {
    JsonValue json;
    json["name"] = std::string(name);
    json["displayName"] = std::string(displayName);
    json["description"] = std::string(description);
    json["author"] = std::string(author);
    json["website"] = std::string(website);
    json["license"] = std::string(license);
    json["version"] = std::string(version.toString());
    json["apiVersion"] = std::string(apiVersion.toString());
    
    return json;
}

PluginMetadata PluginMetadata::fromJson(const JsonValue& json) {
    PluginMetadata metadata;
    
    auto getString = [&](const std::string& key) -> std::string {
        auto it = json.find(key);
        if (it != json.end()) {
            try {
                return std::any_cast<std::string>(it->second);
            } catch (const std::bad_any_cast&) {
                return "";
            }
        }
        return "";
    };
    
    metadata.name = getString("name");
    metadata.displayName = getString("displayName");
    metadata.description = getString("description");
    metadata.author = getString("author");
    metadata.website = getString("website");
    metadata.license = getString("license");
    metadata.version = PluginVersion::fromString(getString("version"));
    const auto apiVersion = getString("apiVersion");
    metadata.apiVersion = apiVersion.empty() ? PluginVersion{1, 0, 0}
                                             : PluginVersion::fromString(apiVersion);
    
    return metadata;
}

bool PluginMetadata::validate() const {
    return getValidationErrors().empty();
}

std::vector<std::string> PluginMetadata::getValidationErrors() const {
    std::vector<std::string> errors;
    if (!validIdentifier(name)) errors.push_back("Plugin name must be a non-empty identifier");
    if (author.empty()) errors.push_back("Plugin author is required");
    if (!validVersion(version)) errors.push_back("Plugin version must be a positive semantic version");
    if (!validVersion(apiVersion) || apiVersion.major != 1) {
        errors.push_back("Plugin API version must be compatible with host API major version 1");
    }

    std::unordered_set<std::string> dependenciesSeen;
    for (const auto& dependency : dependencies) {
        if (!validIdentifier(dependency.pluginName)) {
            errors.push_back("Dependency name must be a non-empty identifier");
        } else if (dependency.pluginName == name) {
            errors.push_back("Plugin cannot depend on itself");
        } else if (!dependenciesSeen.insert(dependency.pluginName).second) {
            errors.push_back("Duplicate dependency: " + dependency.pluginName);
        }
        if (!validVersion(dependency.minVersion)) {
            errors.push_back("Dependency minimum version is invalid: " + dependency.pluginName);
        }
        const bool unbounded = dependency.maxVersion.major == 0 &&
                               dependency.maxVersion.minor == 0 &&
                               dependency.maxVersion.patch == 0;
        if (!unbounded && (!validVersion(dependency.maxVersion) ||
                           compareVersions(dependency.maxVersion, dependency.minVersion) < 0)) {
            errors.push_back("Dependency version range is invalid: " + dependency.pluginName);
        }
    }

    static const std::unordered_set<std::string> parameterTypes{
        "string", "int", "float", "bool", "array", "object"};
    std::unordered_set<std::string> parameterNames;
    for (const auto& parameter : parameters) {
        if (!validIdentifier(parameter.name)) errors.push_back("Parameter name is invalid");
        else if (!parameterNames.insert(parameter.name).second) {
            errors.push_back("Duplicate parameter: " + parameter.name);
        }
        if (parameterTypes.count(parameter.type) == 0) {
            errors.push_back("Unsupported parameter type: " + parameter.type);
        }
    }
    return errors;
}

// =====================================================
// PluginResult Implementation
// =====================================================

JsonValue PluginResult::toJson() const {
    JsonValue json;
    json["success"] = std::string(success ? "true" : "false");
    json["message"] = std::string(message);
    json["executionTime"] = std::string(std::to_string(executionTime.count()) + "ms");
    
    return json;
}

// =====================================================
// PluginInterface Implementation
// =====================================================

PluginResult PluginInterface::handleHook(PluginHook hook, const PluginContext& context) {
    // Default implementation - do nothing
    PluginResult result;
    result.success = true;
    result.message = "Hook " + pluginHookToString(hook) + " handled";
    
    // Use context to avoid warning
    if (context.requestId.empty()) {
        result.message += " (no request ID)";
    }
    
    return result;
}

JsonValue PluginInterface::getStatus() const {
    JsonValue status;
    status["initialized"] = std::string(initialized_.load() ? "true" : "false");
    status["executionCount"] = std::string(std::to_string(executionCount_));
    status["totalExecutionTime"] = std::string(std::to_string(totalExecutionTime_.count()) + "ms");
    
    auto now = std::chrono::system_clock::now();
    auto timeSinceLastExecution = std::chrono::duration_cast<std::chrono::seconds>(now - lastExecuted_).count();
    status["timeSinceLastExecution"] = std::string(std::to_string(timeSinceLastExecution) + "s");
    
    return status;
}

bool PluginInterface::healthCheck() const {
    return initialized_.load(std::memory_order_acquire);
}

bool PluginInterface::isInitialized() const noexcept {
    return initialized_.load(std::memory_order_acquire);
}

bool PluginInterface::validateConfiguration(const std::unordered_map<std::string, std::any>& config) const {
    // The base interface intentionally accepts any configuration. Specialized
    // plugins override this method when they require schema validation.
    (void)config;
    return true;
}

std::vector<PluginCapability> PluginInterface::getCapabilities() const {
    // Default implementation - return capabilities from metadata
    return getMetadata().capabilities;
}

// =====================================================
// SimplePlugin Implementation
// =====================================================

SimplePlugin::SimplePlugin(const PluginMetadata& metadata) : metadata_(metadata) {}

PluginMetadata SimplePlugin::getMetadata() const {
    return metadata_;
}

bool SimplePlugin::initialize(const std::unordered_map<std::string, std::any>& parameters) {
    parameters_ = parameters;
    initialized_ = true;
    return true;
}

void SimplePlugin::shutdown() {
    initialized_ = false;
    parameters_.clear();
}

std::vector<PluginCapability> SimplePlugin::getCapabilities() const {
    return metadata_.capabilities;
}

// =====================================================
// PluginRegistry Implementation
// =====================================================

PluginRegistry::PluginRegistry() = default;

PluginRegistry::~PluginRegistry() {
    std::vector<std::shared_ptr<PluginInterface>> plugins;
    {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        for (auto& entry : plugins_) plugins.push_back(std::move(entry.second));
        plugins_.clear();
        pluginMetadata_.clear();
        dynamicPlugins_.clear();
        unloading_.clear();
    }
    for (const auto& plugin : plugins) {
        if (!plugin) continue;
        try { plugin->shutdown(); } catch (...) {}
    }
}

bool PluginRegistry::registerPlugin(std::shared_ptr<PluginInterface> plugin) {
    if (!plugin) return false;
    PluginMetadata metadata;
    try { metadata = plugin->getMetadata(); } catch (...) { return false; }
    if (!metadata.validate()) return false;

    std::lock_guard<std::mutex> lock(pluginsMutex_);
    if (plugins_.count(metadata.name) != 0 || unloading_.count(metadata.name) != 0) return false;
    for (const auto& dependency : metadata.dependencies) {
        auto found = pluginMetadata_.find(dependency.pluginName);
        if (found == pluginMetadata_.end() || unloading_.count(dependency.pluginName) != 0) {
            if (dependency.required) return false;
            continue;
        }
        if (!dependency.isSatisfiedBy(found->second.version)) return false;
    }
    plugins_.emplace(metadata.name, std::move(plugin));
    pluginMetadata_.emplace(metadata.name, std::move(metadata));
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& pluginName) {
    std::shared_ptr<PluginInterface> plugin;
    {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end() || unloading_.count(pluginName) != 0) return false;
        for (const auto& entry : pluginMetadata_) {
            if (entry.first == pluginName) continue;
            for (const auto& dependency : entry.second.dependencies) {
                if (dependency.required && dependency.pluginName == pluginName) return false;
            }
        }
        plugin = it->second;
        unloading_.insert(pluginName);
    }
    try {
        plugin->shutdown();
        if (plugin->isInitialized()) throw std::runtime_error("plugin remained initialized");
    } catch (...) {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        unloading_.erase(pluginName);
        return false;
    }
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end() || it->second != plugin) {
        unloading_.erase(pluginName);
        return false;
    }
    plugins_.erase(it);
    pluginMetadata_.erase(pluginName);
    dynamicPlugins_.erase(pluginName);
    unloading_.erase(pluginName);
    return true;
}

std::shared_ptr<PluginInterface> PluginRegistry::getPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    auto it = plugins_.find(pluginName);
    return it != plugins_.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<PluginInterface>> PluginRegistry::getAllPlugins() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    std::vector<std::shared_ptr<PluginInterface>> result;
    result.reserve(plugins_.size());
    for (const auto& entry : plugins_) result.push_back(entry.second);
    return result;
}

std::vector<std::shared_ptr<PluginInterface>>
PluginRegistry::getPluginsByCapability(PluginCapability capability) const {
    std::vector<std::shared_ptr<PluginInterface>> result;
    for (const auto& plugin : getAllPlugins()) {
        try {
            const auto capabilities = plugin->getCapabilities();
            if (std::find(capabilities.begin(), capabilities.end(), capability) != capabilities.end()) {
                result.push_back(plugin);
            }
        } catch (...) {}
    }
    return result;
}

std::vector<PluginMetadata> PluginRegistry::discoverPlugins(const std::string& directory) const {
    std::vector<PluginMetadata> discovered;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!entry.is_regular_file()) continue;
#ifdef _WIN32
            const bool library = entry.path().extension() == ".dll";
#elif defined(__APPLE__)
            const bool library = entry.path().extension() == ".dylib";
#else
            const bool library = entry.path().extension() == ".so";
#endif
            if (!library) continue;
            PluginMetadata metadata;
            metadata.name = entry.path().stem().string();
            metadata.author = "Unknown";
            metadata.version = PluginVersion{1, 0, 0};
            discovered.push_back(metadata);
        }
    } catch (const std::filesystem::filesystem_error&) {}
    return discovered;
}

std::shared_ptr<PluginInterface> PluginRegistry::loadPlugin(const std::string& pluginPath) {
    if (pluginPath.empty() || !std::filesystem::is_regular_file(pluginPath)) return nullptr;
#ifdef _WIN32
    HMODULE nativeHandle = LoadLibraryA(pluginPath.c_str());
    if (!nativeHandle) return nullptr;
    using PluginCreateFunc = PluginInterface* (*)();
    auto create = reinterpret_cast<PluginCreateFunc>(
        GetProcAddress(nativeHandle, "elizaos_create_plugin"));
    if (!create) { FreeLibrary(nativeHandle); return nullptr; }
    void* handle = reinterpret_cast<void*>(nativeHandle);
#else
    void* handle = dlopen(pluginPath.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) return nullptr;
    using PluginCreateFunc = PluginInterface* (*)();
    dlerror();
    auto create = reinterpret_cast<PluginCreateFunc>(dlsym(handle, "elizaos_create_plugin"));
    if (dlerror() != nullptr || !create) { dlclose(handle); return nullptr; }
#endif

    PluginInterface* raw = nullptr;
    try { raw = create(); } catch (...) { raw = nullptr; }
    if (!raw) {
#ifdef _WIN32
        FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        return nullptr;
    }
    std::shared_ptr<PluginInterface> plugin(raw, [handle](PluginInterface* value) {
        delete value;
#ifdef _WIN32
        if (handle) FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
        if (handle) dlclose(handle);
#endif
    });

    PluginMetadata metadata;
    try { metadata = plugin->getMetadata(); } catch (...) { return nullptr; }
    if (!metadata.validate()) return nullptr;
    {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        if (plugins_.count(metadata.name) != 0 || unloading_.count(metadata.name) != 0) {
            return nullptr;
        }
        for (const auto& dependency : metadata.dependencies) {
            auto found = pluginMetadata_.find(dependency.pluginName);
            if (found == pluginMetadata_.end() || unloading_.count(dependency.pluginName) != 0) {
                if (dependency.required) return nullptr;
                continue;
            }
            if (!dependency.isSatisfiedBy(found->second.version)) return nullptr;
        }
        plugins_.emplace(metadata.name, plugin);
        pluginMetadata_.emplace(metadata.name, metadata);
        dynamicPlugins_.emplace(metadata.name, DynamicPluginInfo{
            handle, pluginPath, std::chrono::system_clock::now()});
    }
    return plugin;
}

bool PluginRegistry::unloadPlugin(const std::string& pluginName) {
    {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        if (dynamicPlugins_.count(pluginName) == 0) return false;
    }
    return unregisterPlugin(pluginName);
}

bool PluginRegistry::isDynamicPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    return dynamicPlugins_.count(pluginName) != 0;
}

std::vector<std::string> PluginRegistry::getDynamicPlugins() const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    std::vector<std::string> result;
    for (const auto& entry : dynamicPlugins_) result.push_back(entry.first);
    std::sort(result.begin(), result.end());
    return result;
}

bool PluginRegistry::hotReloadPlugin(const std::string& pluginName) {
    std::string path;
    {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        auto it = dynamicPlugins_.find(pluginName);
        if (it == dynamicPlugins_.end()) return false;
        path = it->second.path;
    }
    return unloadPlugin(pluginName) && loadPlugin(path) != nullptr;
}

bool PluginRegistry::validateDependencies(const PluginMetadata& metadata) const {
    std::lock_guard<std::mutex> lock(pluginsMutex_);
    for (const auto& dependency : metadata.dependencies) {
        auto found = pluginMetadata_.find(dependency.pluginName);
        if (found == pluginMetadata_.end() || unloading_.count(dependency.pluginName) != 0) {
            if (dependency.required) return false;
            continue;
        }
        if (!dependency.isSatisfiedBy(found->second.version)) return false;
    }
    return true;
}

std::vector<std::string> PluginRegistry::getDependencyOrder() const {
    std::unordered_map<std::string, PluginMetadata> metadata;
    {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        metadata = pluginMetadata_;
    }
    std::vector<std::string> names;
    for (const auto& entry : metadata) names.push_back(entry.first);
    std::sort(names.begin(), names.end());
    std::unordered_map<std::string, int> marks;
    std::vector<std::string> order;
    std::function<bool(const std::string&)> visit = [&](const std::string& name) {
        if (marks[name] == 2) return true;
        if (marks[name] == 1) return false;
        marks[name] = 1;
        std::vector<std::string> dependencies;
        for (const auto& dependency : metadata[name].dependencies) {
            if (metadata.count(dependency.pluginName) != 0) dependencies.push_back(dependency.pluginName);
            else if (dependency.required) return false;
        }
        std::sort(dependencies.begin(), dependencies.end());
        for (const auto& dependency : dependencies) if (!visit(dependency)) return false;
        marks[name] = 2;
        order.push_back(name);
        return true;
    };
    for (const auto& name : names) if (!visit(name)) return {};
    return order;
}

JsonValue PluginRegistry::getStatistics() const {
    const auto plugins = getAllPlugins();
    std::unordered_set<PluginCapability> capabilities;
    for (const auto& plugin : plugins) {
        try {
            for (const auto capability : plugin->getCapabilities()) capabilities.insert(capability);
        } catch (...) {}
    }
    JsonValue stats;
    stats["totalPlugins"] = std::string(std::to_string(plugins.size()));
    stats["capabilityCounts"] = std::string(std::to_string(capabilities.size()));
    return stats;
}

bool PluginRegistry::validatePlugin(std::shared_ptr<PluginInterface> plugin) const {
    if (!plugin) return false;
    try { return plugin->getMetadata().validate(); } catch (...) { return false; }
}

// =====================================================
// PluginManager Implementation
// =====================================================

PluginManager::PluginManager() = default;
PluginManager::~PluginManager() { shutdownAll(); }

void PluginManager::setRegistry(std::shared_ptr<PluginRegistry> registry) {
    std::shared_ptr<PluginRegistry> previous;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        previous = registry_;
    }
    if (previous && previous != registry) shutdownAll();
    std::lock_guard<std::mutex> lock(managerMutex_);
    registry_ = std::move(registry);
    enabledPlugins_.clear();
    configurations_.clear();
    transitioningPlugins_.clear();
}

bool PluginManager::initializeAll(
    const std::unordered_map<std::string, std::unordered_map<std::string, std::any>>& configurations) {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        registry = registry_;
    }
    if (!registry) return false;
    const auto order = registry->getDependencyOrder();
    const auto allPlugins = registry->getAllPlugins();
    if (!allPlugins.empty() && order.size() != allPlugins.size()) return false;
    std::unordered_map<std::string, std::shared_ptr<PluginInterface>> plugins;
    for (const auto& plugin : allPlugins) {
        try { plugins.emplace(plugin->getMetadata().name, plugin); } catch (...) { return false; }
    }

    std::vector<std::string> initialized;
    for (const auto& name : order) {
        auto pluginIt = plugins.find(name);
        if (pluginIt == plugins.end()) return false;
        auto configIt = configurations.find(name);
        const std::unordered_map<std::string, std::any> config =
            configIt == configurations.end() ? std::unordered_map<std::string, std::any>{}
                                             : configIt->second;
        {
            std::lock_guard<std::mutex> lock(managerMutex_);
            if (transitioningPlugins_.count(name) != 0 || enabledPlugins_[name]) return false;
            transitioningPlugins_.insert(name);
        }
        bool success = false;
        try {
            success = pluginIt->second->validateConfiguration(config) &&
                      pluginIt->second->initialize(config);
        } catch (...) { success = false; }
        if (success) pluginIt->second->initialized_.store(true, std::memory_order_release);
        {
            std::lock_guard<std::mutex> lock(managerMutex_);
            transitioningPlugins_.erase(name);
            enabledPlugins_[name] = success;
            if (success) configurations_[name] = config;
        }
        if (!success) {
            bool failedPluginShutdown = true;
            try { pluginIt->second->shutdown(); } catch (...) { failedPluginShutdown = false; }
            if (failedPluginShutdown) {
                pluginIt->second->initialized_.store(false, std::memory_order_release);
            }
            for (auto it = initialized.rbegin(); it != initialized.rend(); ++it) {
                const auto initializedPlugin = plugins.at(*it);
                bool shutdownSucceeded = true;
                try { initializedPlugin->shutdown(); } catch (...) { shutdownSucceeded = false; }
                if (shutdownSucceeded) {
                    initializedPlugin->initialized_.store(false, std::memory_order_release);
                }
                std::lock_guard<std::mutex> lock(managerMutex_);
                enabledPlugins_[*it] = !shutdownSucceeded;
                if (shutdownSucceeded) configurations_.erase(*it);
            }
            return false;
        }
        initialized.push_back(name);
    }
    return true;
}

void PluginManager::shutdownAll() {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        registry = registry_;
    }
    if (!registry) return;
    auto order = registry->getDependencyOrder();
    std::reverse(order.begin(), order.end());
    for (const auto& name : order) {
        auto plugin = registry->getPlugin(name);
        if (!plugin) continue;
        {
            std::lock_guard<std::mutex> lock(managerMutex_);
            if (transitioningPlugins_.count(name) != 0 || !enabledPlugins_[name]) continue;
            transitioningPlugins_.insert(name);
        }
        bool success = true;
        try { plugin->shutdown(); } catch (...) { success = false; }
        if (success) plugin->initialized_.store(false, std::memory_order_release);
        std::lock_guard<std::mutex> lock(managerMutex_);
        if (success) enabledPlugins_[name] = false;
        transitioningPlugins_.erase(name);
    }
}

std::vector<PluginResult> PluginManager::executeHook(PluginHook hook,
                                                      const PluginContext& context) {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        registry = registry_;
    }
    std::vector<PluginResult> results;
    if (!registry) return results;
    for (const auto& plugin : registry->getAllPlugins()) {
        std::string name;
        try { name = plugin->getMetadata().name; } catch (...) { continue; }
        {
            std::lock_guard<std::mutex> lock(managerMutex_);
            if (!enabledPlugins_[name] || transitioningPlugins_.count(name) != 0) continue;
        }
        const auto started = std::chrono::steady_clock::now();
        PluginResult result;
        try { result = plugin->handleHook(hook, context); }
        catch (const std::exception& ex) { result.success = false; result.message = ex.what(); }
        catch (...) { result.success = false; result.message = "unknown exception"; }
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - started);
        {
            std::lock_guard<std::mutex> lock(managerMutex_);
            ++executionCounts_[name];
            executionTimes_[name] += result.executionTime;
            if (!result.success) ++errorCounts_[name];
        }
        results.push_back(std::move(result));
    }
    return results;
}

PluginResult PluginManager::executePlugin(const std::string& pluginName,
                                           const PluginContext& context) {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        registry = registry_;
        if (!registry) { PluginResult failure; failure.success = false; failure.message = "No plugin registry available"; return failure; }
        if (!enabledPlugins_[pluginName] || transitioningPlugins_.count(pluginName) != 0) {
            PluginResult failure; failure.success = false; failure.message = "Plugin is disabled: " + pluginName; return failure;
        }
    }
    auto plugin = registry->getPlugin(pluginName);
    if (!plugin) { PluginResult failure; failure.success = false; failure.message = "Plugin not found: " + pluginName; return failure; }
    const auto started = std::chrono::steady_clock::now();
    PluginResult result;
    try { result = plugin->execute(context); }
    catch (const std::exception& ex) { result.success = false; result.message = ex.what(); }
    catch (...) { result.success = false; result.message = "unknown exception"; }
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - started);
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        ++executionCounts_[pluginName];
        executionTimes_[pluginName] += result.executionTime;
        if (!result.success) ++errorCounts_[pluginName];
    }
    return result;
}

std::vector<PluginResult> PluginManager::executeByCapability(PluginCapability capability,
                                                              const PluginContext& context) {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        registry = registry_;
    }
    std::vector<PluginResult> results;
    if (!registry) return results;
    for (const auto& plugin : registry->getPluginsByCapability(capability)) {
        try { results.push_back(executePlugin(plugin->getMetadata().name, context)); }
        catch (...) {}
    }
    return results;
}

JsonValue PluginManager::getExecutionStats() const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    size_t totalExecutions = 0, totalErrors = 0;
    for (const auto& entry : executionCounts_) totalExecutions += entry.second;
    for (const auto& entry : errorCounts_) totalErrors += entry.second;
    JsonValue stats;
    stats["totalPlugins"] = std::string(std::to_string(enabledPlugins_.size()));
    stats["totalExecutions"] = std::string(std::to_string(totalExecutions));
    stats["totalErrors"] = std::string(std::to_string(totalErrors));
    stats["errorRate"] = std::string(totalExecutions == 0 ? "0.0" :
        std::to_string(static_cast<double>(totalErrors) / totalExecutions));
    return stats;
}

bool PluginManager::setPluginEnabled(const std::string& pluginName, bool enabled) {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        registry = registry_;
    }
    if (!registry || !registry->getPlugin(pluginName)) return false;
    std::lock_guard<std::mutex> lock(managerMutex_);
    if (transitioningPlugins_.count(pluginName) != 0) return false;
    if (enabled && !configurations_.count(pluginName)) return false;
    enabledPlugins_[pluginName] = enabled;
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    auto it = enabledPlugins_.find(pluginName);
    return it != enabledPlugins_.end() && it->second &&
           transitioningPlugins_.count(pluginName) == 0;
}

bool PluginManager::checkPluginHealth(const std::string& pluginName) const {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        auto enabled = enabledPlugins_.find(pluginName);
        if (enabled == enabledPlugins_.end() || !enabled->second ||
            transitioningPlugins_.count(pluginName) != 0) return false;
        registry = registry_;
    }
    if (!registry) return false;
    auto plugin = registry->getPlugin(pluginName);
    if (!plugin || !plugin->isInitialized()) return false;
    try { return plugin->healthCheck(); } catch (...) { return false; }
}

std::unordered_map<std::string, std::any>
PluginManager::getPluginConfiguration(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    auto it = configurations_.find(pluginName);
    return it != configurations_.end() ? it->second
                                       : std::unordered_map<std::string, std::any>{};
}

bool PluginManager::updatePluginConfiguration(
    const std::string& pluginName,
    const std::unordered_map<std::string, std::any>& config) {
    std::shared_ptr<PluginRegistry> registry;
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        registry = registry_;
        if (transitioningPlugins_.count(pluginName) != 0) return false;
    }
    if (!registry) return false;
    auto plugin = registry->getPlugin(pluginName);
    if (!plugin) return false;
    bool valid = false;
    try { valid = plugin->validateConfiguration(config); } catch (...) { return false; }
    if (!valid) return false;
    std::lock_guard<std::mutex> lock(managerMutex_);
    configurations_[pluginName] = config;
    return true;
}

// =====================================================
// PluginFactory Implementation  
// =====================================================

std::unordered_map<std::string, PluginFactory::PluginCreator> PluginFactory::creators_;
std::mutex PluginFactory::creatorsMutex_;

void PluginFactory::registerPlugin(const std::string& pluginName, PluginCreator creator) {
    std::lock_guard<std::mutex> lock(creatorsMutex_);
    creators_[pluginName] = creator;
}

std::shared_ptr<PluginInterface> PluginFactory::createPlugin(const std::string& pluginName) {
    PluginCreator creator;
    {
        std::lock_guard<std::mutex> lock(creatorsMutex_);
        auto it = creators_.find(pluginName);
        if (it == creators_.end()) return nullptr;
        creator = it->second;
    }
    try { return creator(); } catch (...) { return nullptr; }
}

std::vector<std::string> PluginFactory::getRegisteredPlugins() {
    std::lock_guard<std::mutex> lock(creatorsMutex_);
    
    std::vector<std::string> plugins;
    for (const auto& [key, val] : creators_) {
        plugins.push_back(key);
    }
    
    return plugins;
}

// =====================================================
// Utility Functions
// =====================================================

std::string pluginCapabilityToString(PluginCapability capability) {
    switch (capability) {
        case PluginCapability::ACTION_PROCESSING: return "action_processing";
        case PluginCapability::MESSAGE_HANDLING: return "message_handling";
        case PluginCapability::KNOWLEDGE_EXPANSION: return "knowledge_expansion";
        case PluginCapability::CONVERSATION_FLOW: return "conversation_flow";
        case PluginCapability::MEMORY_INTEGRATION: return "memory_integration";
        case PluginCapability::EXTERNAL_API: return "external_api";
        case PluginCapability::DATA_TRANSFORMATION: return "data_transformation";
        case PluginCapability::AUTHENTICATION: return "authentication";
        case PluginCapability::ANALYTICS: return "analytics";
        case PluginCapability::CUSTOM: return "custom";
        default: return "unknown";
    }
}

PluginCapability stringToPluginCapability(const std::string& capabilityStr) {
    if (capabilityStr == "action_processing") return PluginCapability::ACTION_PROCESSING;
    if (capabilityStr == "message_handling") return PluginCapability::MESSAGE_HANDLING;
    if (capabilityStr == "knowledge_expansion") return PluginCapability::KNOWLEDGE_EXPANSION;
    if (capabilityStr == "conversation_flow") return PluginCapability::CONVERSATION_FLOW;
    if (capabilityStr == "memory_integration") return PluginCapability::MEMORY_INTEGRATION;
    if (capabilityStr == "external_api") return PluginCapability::EXTERNAL_API;
    if (capabilityStr == "data_transformation") return PluginCapability::DATA_TRANSFORMATION;
    if (capabilityStr == "authentication") return PluginCapability::AUTHENTICATION;
    if (capabilityStr == "analytics") return PluginCapability::ANALYTICS;
    if (capabilityStr == "custom") return PluginCapability::CUSTOM;
    return PluginCapability::CUSTOM;
}

std::string pluginHookToString(PluginHook hook) {
    switch (hook) {
        case PluginHook::BEFORE_MESSAGE_PROCESSING: return "before_message_processing";
        case PluginHook::AFTER_MESSAGE_PROCESSING: return "after_message_processing";
        case PluginHook::BEFORE_RESPONSE_GENERATION: return "before_response_generation";
        case PluginHook::AFTER_RESPONSE_GENERATION: return "after_response_generation";
        case PluginHook::BEFORE_MEMORY_STORAGE: return "before_memory_storage";
        case PluginHook::AFTER_MEMORY_STORAGE: return "after_memory_storage";
        case PluginHook::BEFORE_ACTION_EXECUTION: return "before_action_execution";
        case PluginHook::AFTER_ACTION_EXECUTION: return "after_action_execution";
        case PluginHook::SESSION_START: return "session_start";
        case PluginHook::SESSION_END: return "session_end";
        case PluginHook::AGENT_STARTUP: return "agent_startup";
        case PluginHook::AGENT_SHUTDOWN: return "agent_shutdown";
        default: return "unknown";
    }
}

PluginHook stringToPluginHook(const std::string& hookStr) {
    if (hookStr == "before_message_processing") return PluginHook::BEFORE_MESSAGE_PROCESSING;
    if (hookStr == "after_message_processing") return PluginHook::AFTER_MESSAGE_PROCESSING;
    if (hookStr == "before_response_generation") return PluginHook::BEFORE_RESPONSE_GENERATION;
    if (hookStr == "after_response_generation") return PluginHook::AFTER_RESPONSE_GENERATION;
    if (hookStr == "before_memory_storage") return PluginHook::BEFORE_MEMORY_STORAGE;
    if (hookStr == "after_memory_storage") return PluginHook::AFTER_MEMORY_STORAGE;
    if (hookStr == "before_action_execution") return PluginHook::BEFORE_ACTION_EXECUTION;
    if (hookStr == "after_action_execution") return PluginHook::AFTER_ACTION_EXECUTION;
    if (hookStr == "session_start") return PluginHook::SESSION_START;
    if (hookStr == "session_end") return PluginHook::SESSION_END;
    if (hookStr == "agent_startup") return PluginHook::AGENT_STARTUP;
    if (hookStr == "agent_shutdown") return PluginHook::AGENT_SHUTDOWN;
    return PluginHook::AGENT_STARTUP;
}

} // namespace elizaos
