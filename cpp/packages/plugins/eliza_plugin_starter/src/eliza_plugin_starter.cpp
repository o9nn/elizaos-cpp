#include "elizaos/eliza_plugin_starter.hpp"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <cmath>
#include <iostream>
#include <set>
#include <utility>

namespace elizaos {
namespace {

bool validIdentifier(const std::string& value) {
    if (value.empty() || value.size() > 128U) {
        return false;
    }
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isalnum(ch) != 0 || ch == '-' || ch == '_' || ch == '.';
    });
}

bool parseVersionPart(const char*& begin, const char* end, int& value) {
    if (begin == end || std::isdigit(static_cast<unsigned char>(*begin)) == 0) {
        return false;
    }
    if (*begin == '0' && begin + 1 != end &&
        std::isdigit(static_cast<unsigned char>(*(begin + 1))) != 0) {
        return false;
    }
    const auto result = std::from_chars(begin, end, value);
    if (result.ec != std::errc{} || value < 0) {
        return false;
    }
    begin = result.ptr;
    return true;
}

void shutdownPlugin(const std::shared_ptr<IPlugin>& plugin, PluginState finalState) noexcept {
    if (!plugin) {
        return;
    }
    try {
        plugin->shutdown();
    } catch (...) {
    }
    try {
        plugin->setState(finalState);
    } catch (...) {
    }
}

} // namespace

std::string PluginVersion::toString() const {
    return std::to_string(major) + "." + std::to_string(minor) + "." +
           std::to_string(patch);
}

bool PluginVersion::isValid() const {
    return major >= 0 && minor >= 0 && patch >= 0;
}

bool PluginVersion::isCompatibleWith(const PluginVersion& other) const {
    return isValid() && other.isValid() && major == other.major;
}

std::optional<PluginVersion> PluginVersion::tryParse(const std::string& versionStr) {
    const char* cursor = versionStr.data();
    const char* const end = cursor + versionStr.size();
    PluginVersion version{0, 0, 0};
    if (!parseVersionPart(cursor, end, version.major) || cursor == end || *cursor++ != '.' ||
        !parseVersionPart(cursor, end, version.minor) || cursor == end || *cursor++ != '.' ||
        !parseVersionPart(cursor, end, version.patch) || cursor != end) {
        return std::nullopt;
    }
    return version;
}

PluginVersion PluginVersion::parse(const std::string& versionStr) {
    return tryParse(versionStr).value_or(PluginVersion{-1, -1, -1});
}

PluginConfig::PluginConfig(const PluginConfig& other) {
    std::lock_guard<std::mutex> lock(other.mutex_);
    values_ = other.values_;
    requiredKeys_ = other.requiredKeys_;
}

PluginConfig& PluginConfig::operator=(const PluginConfig& other) {
    if (this == &other) {
        return *this;
    }

    std::unordered_map<std::string, std::any> values;
    std::vector<std::string> requiredKeys;
    {
        std::lock_guard<std::mutex> lock(other.mutex_);
        values = other.values_;
        requiredKeys = other.requiredKeys_;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        values_.swap(values);
        requiredKeys_.swap(requiredKeys);
    }
    return *this;
}

PluginConfig::PluginConfig(PluginConfig&& other) {
    std::lock_guard<std::mutex> lock(other.mutex_);
    values_ = std::move(other.values_);
    requiredKeys_ = std::move(other.requiredKeys_);
}

PluginConfig& PluginConfig::operator=(PluginConfig&& other) {
    if (this == &other) {
        return *this;
    }

    std::unordered_map<std::string, std::any> values;
    std::vector<std::string> requiredKeys;
    {
        std::lock_guard<std::mutex> lock(other.mutex_);
        values = std::move(other.values_);
        requiredKeys = std::move(other.requiredKeys_);
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        values_.swap(values);
        requiredKeys_.swap(requiredKeys);
    }
    return *this;
}

void PluginConfig::set(const std::string& key, const std::string& value) {
    if (key.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    values_[key] = value;
}

void PluginConfig::set(const std::string& key, const char* value) {
    set(key, value != nullptr ? std::string(value) : std::string{});
}

void PluginConfig::set(const std::string& key, int value) {
    if (key.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    values_[key] = value;
}

void PluginConfig::set(const std::string& key, double value) {
    if (key.empty() || !std::isfinite(value)) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    values_[key] = value;
}

void PluginConfig::set(const std::string& key, bool value) {
    if (key.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    values_[key] = value;
}

std::string PluginConfig::getString(const std::string& key,
                                    const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = values_.find(key);
    if (it == values_.end()) {
        return defaultValue;
    }
    const auto* value = std::any_cast<std::string>(&it->second);
    return value != nullptr ? *value : defaultValue;
}

int PluginConfig::getInt(const std::string& key, int defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = values_.find(key);
    if (it == values_.end()) {
        return defaultValue;
    }
    const auto* value = std::any_cast<int>(&it->second);
    return value != nullptr ? *value : defaultValue;
}

double PluginConfig::getDouble(const std::string& key, double defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = values_.find(key);
    if (it == values_.end()) {
        return defaultValue;
    }
    const auto* value = std::any_cast<double>(&it->second);
    return value != nullptr ? *value : defaultValue;
}

bool PluginConfig::getBool(const std::string& key, bool defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = values_.find(key);
    if (it == values_.end()) {
        return defaultValue;
    }
    const auto* value = std::any_cast<bool>(&it->second);
    return value != nullptr ? *value : defaultValue;
}

bool PluginConfig::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return values_.find(key) != values_.end();
}

bool PluginConfig::validate(std::string& errorMessage) const {
    std::lock_guard<std::mutex> lock(mutex_);
    errorMessage.clear();
    for (const auto& key : requiredKeys_) {
        if (values_.find(key) == values_.end()) {
            errorMessage = "Missing required key: " + key;
            return false;
        }
    }
    return true;
}

void PluginConfig::addRequiredKey(const std::string& key) {
    if (key.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (std::find(requiredKeys_.begin(), requiredKeys_.end(), key) == requiredKeys_.end()) {
        requiredKeys_.push_back(key);
    }
}

PluginContext::PluginContext(const std::string& pluginId) : pluginId_(pluginId) {}

void PluginContext::setAgentState(std::shared_ptr<State> state) {
    std::shared_ptr<State> previous;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        previous = std::move(agentState_);
        agentState_ = std::move(state);
    }
}

std::shared_ptr<State> PluginContext::getAgentState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return agentState_;
}

void PluginContext::log(const std::string& message) const {
    std::cout << "[" << pluginId_ << "] " << message << '\n';
}

void PluginContext::logWarning(const std::string& message) const {
    std::cerr << "[" << pluginId_ << "] WARNING: " << message << '\n';
}

void PluginContext::logError(const std::string& message) const {
    std::cerr << "[" << pluginId_ << "] ERROR: " << message << '\n';
}

void PluginContext::setData(const std::string& key, std::any value) {
    if (key.empty()) {
        return;
    }
    auto storedValue = std::make_shared<const std::any>(std::move(value));
    std::shared_ptr<const std::any> previous;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& slot = pluginData_[key];
        previous = std::move(slot);
        slot = std::move(storedValue);
    }
}

std::any PluginContext::getData(const std::string& key) const {
    std::shared_ptr<const std::any> value;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = pluginData_.find(key);
        if (it == pluginData_.end()) {
            return {};
        }
        value = it->second;
    }
    return *value;
}

bool PluginContext::hasData(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pluginData_.find(key) != pluginData_.end();
}

PluginRegistry& PluginRegistry::instance() {
    // Intentionally process-lifetime storage: registration macros may execute
    // during static initialization/destruction in otherwise unspecified order.
    static PluginRegistry* const registry = new PluginRegistry();
    return *registry;
}

void PluginRegistry::registerPlugin(const std::string& pluginId, PluginFactory factory) {
    if (!validIdentifier(pluginId) || !factory) {
        return;
    }
    auto storedFactory = std::make_shared<const PluginFactory>(std::move(factory));
    std::shared_ptr<const PluginFactory> previous;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& slot = factories_[pluginId];
        previous = std::move(slot);
        slot = std::move(storedFactory);
    }
}

bool PluginRegistry::tryRegisterPlugin(const std::string& pluginId, PluginFactory factory) {
    if (!validIdentifier(pluginId) || !factory) {
        return false;
    }
    auto storedFactory = std::make_shared<const PluginFactory>(std::move(factory));
    std::lock_guard<std::mutex> lock(mutex_);
    if (factories_.find(pluginId) != factories_.end()) {
        return false;
    }
    factories_.emplace(pluginId, std::move(storedFactory));
    return true;
}

void PluginRegistry::unregisterPlugin(const std::string& pluginId) {
    std::shared_ptr<const PluginFactory> removed;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = factories_.find(pluginId);
        if (it == factories_.end()) {
            return;
        }
        removed = std::move(it->second);
        factories_.erase(it);
    }
}

std::unique_ptr<IPlugin> PluginRegistry::createPlugin(const std::string& pluginId) const {
    std::shared_ptr<const PluginFactory> factory;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = factories_.find(pluginId);
        if (it == factories_.end()) {
            return nullptr;
        }
        factory = it->second;
    }
    try {
        return factory != nullptr && *factory ? (*factory)() : nullptr;
    } catch (...) {
        return nullptr;
    }
}

bool PluginRegistry::hasPlugin(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return factories_.find(pluginId) != factories_.end();
}

std::vector<std::string> PluginRegistry::getRegisteredPluginIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    ids.reserve(factories_.size());
    for (const auto& entry : factories_) {
        ids.push_back(entry.first);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

void PluginRegistry::clear() {
    std::unordered_map<std::string, std::shared_ptr<const PluginFactory>> removed;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        removed.swap(factories_);
    }
}

PluginManager::PluginManager() = default;

PluginManager::~PluginManager() {
    unloadAllInternal(true);
}

bool PluginManager::currentThreadOwnsTransitionLocked() const {
    const auto currentThread = std::this_thread::get_id();
    return std::any_of(transitioning_.begin(), transitioning_.end(),
                       [&currentThread](const auto& transition) {
                           return transition.second == currentThread;
                       });
}

void PluginManager::finishTransitionLocked(const std::string& pluginId) {
    transitioning_.erase(pluginId);
    transitionCondition_.notify_all();
}

bool PluginManager::hasDependentsLocked(const std::string& pluginId) const {
    for (const auto& entry : dependencies_) {
        if (entry.first != pluginId &&
            std::find(entry.second.begin(), entry.second.end(), pluginId) != entry.second.end()) {
            return true;
        }
    }
    return false;
}

bool PluginManager::loadPlugin(const std::string& pluginId, const PluginConfig& config) {
    if (!validIdentifier(pluginId)) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (shuttingDown_ || unloadingAll_ || plugins_.count(pluginId) != 0U ||
            transitioning_.count(pluginId) != 0U) {
            return false;
        }
        transitioning_.emplace(pluginId, std::this_thread::get_id());
    }

    const auto finishTransition = [this, &pluginId]() {
        std::lock_guard<std::mutex> lock(mutex_);
        finishTransitionLocked(pluginId);
    };

    auto uniquePlugin = PluginRegistry::instance().createPlugin(pluginId);
    if (!uniquePlugin) {
        finishTransition();
        return false;
    }
    std::shared_ptr<IPlugin> plugin(std::move(uniquePlugin));

    PluginMetadata metadata;
    std::string metadataError;
    try {
        metadata = plugin->getMetadata();
    } catch (...) {
        finishTransition();
        return false;
    }
    if (!PluginUtils::validateMetadata(metadata, metadataError) || metadata.id != pluginId) {
        finishTransition();
        return false;
    }

    std::shared_ptr<State> state;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& dependency : metadata.dependencies) {
            if (plugins_.count(dependency) == 0U || transitioning_.count(dependency) != 0U) {
                finishTransitionLocked(pluginId);
                return false;
            }
        }
        state = agentState_;
    }

    auto managed = std::make_shared<ManagedPlugin>();
    managed->context = std::make_shared<PluginContext>(pluginId);
    managed->plugin = plugin;
    managed->dependencies = metadata.dependencies;
    managed->context->setAgentState(std::move(state));

    bool initialized = false;
    try {
        plugin->setState(PluginState::LOADED);
        initialized = plugin->initialize(config, *managed->context);
    } catch (...) {
        initialized = false;
    }
    if (!initialized) {
        shutdownPlugin(plugin, PluginState::ERROR);
        finishTransition();
        return false;
    }

    try {
        plugin->setState(PluginState::ACTIVE);
    } catch (...) {
        shutdownPlugin(plugin, PluginState::ERROR);
        finishTransition();
        return false;
    }

    bool inserted = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        bool dependenciesStillAvailable = true;
        for (const auto& dependency : metadata.dependencies) {
            if (plugins_.count(dependency) == 0U || transitioning_.count(dependency) != 0U) {
                dependenciesStillAvailable = false;
                break;
            }
        }
        if (!shuttingDown_ && !unloadingAll_ && dependenciesStillAvailable &&
            plugins_.count(pluginId) == 0U) {
            plugins_.emplace(pluginId, managed);
            dependencies_.emplace(pluginId, metadata.dependencies);
            inserted = true;
            finishTransitionLocked(pluginId);
        }
    }

    if (!inserted) {
        shutdownPlugin(plugin, PluginState::UNLOADED);
        finishTransition();
    }
    return inserted;
}

bool PluginManager::unloadPluginInternal(const std::string& pluginId, bool duringUnloadAll) {
    std::shared_ptr<ManagedPlugin> managed;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if ((!duringUnloadAll && (shuttingDown_ || unloadingAll_)) ||
            transitioning_.count(pluginId) != 0U) {
            return false;
        }
        const auto it = plugins_.find(pluginId);
        if (it == plugins_.end() || hasDependentsLocked(pluginId)) {
            return false;
        }
        transitioning_.emplace(pluginId, std::this_thread::get_id());
        managed = it->second;
        plugins_.erase(it);
    }

    shutdownPlugin(managed->plugin, PluginState::UNLOADED);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        dependencies_.erase(pluginId);
        retiredPlugins_.push_back(std::move(managed));
        finishTransitionLocked(pluginId);
    }
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginId) {
    return unloadPluginInternal(pluginId, false);
}

void PluginManager::unloadAllInternal(bool duringDestruction) {
    const auto currentThread = std::this_thread::get_id();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (duringDestruction) {
            shuttingDown_ = true;
        }
        if (unloadingAll_) {
            if (unloadingThread_ == currentThread) {
                return;
            }
            transitionCondition_.wait(lock, [this] { return !unloadingAll_; });
            if (!duringDestruction) {
                return;
            }
        }
        if (currentThreadOwnsTransitionLocked()) {
            return;
        }
        unloadingAll_ = true;
        unloadingThread_ = currentThread;
        transitionCondition_.wait(lock, [this] { return transitioning_.empty(); });
    }

    for (;;) {
        std::vector<std::string> leaves;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (plugins_.empty()) {
                break;
            }
            leaves.reserve(plugins_.size());
            for (const auto& entry : plugins_) {
                if (!hasDependentsLocked(entry.first)) {
                    leaves.push_back(entry.first);
                }
            }
        }

        if (leaves.empty()) {
            // Valid metadata cannot create self dependencies, and load order
            // prevents manager-created cycles. This fallback makes teardown
            // total even if internal state is corrupted.
            std::vector<std::shared_ptr<ManagedPlugin>> forced;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                forced.reserve(plugins_.size());
                for (auto& entry : plugins_) {
                    forced.push_back(std::move(entry.second));
                }
                plugins_.clear();
                dependencies_.clear();
            }
            for (const auto& managed : forced) {
                shutdownPlugin(managed->plugin, PluginState::UNLOADED);
            }
            {
                std::lock_guard<std::mutex> lock(mutex_);
                for (auto& managed : forced) {
                    retiredPlugins_.push_back(std::move(managed));
                }
            }
            break;
        }

        std::sort(leaves.begin(), leaves.end());
        for (const auto& pluginId : leaves) {
            (void)unloadPluginInternal(pluginId, true);
        }
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        unloadingAll_ = false;
        unloadingThread_ = {};
        transitionCondition_.notify_all();
    }
}

void PluginManager::unloadAll() {
    unloadAllInternal(false);
}

std::shared_ptr<IPlugin> PluginManager::getPluginShared(const std::string& pluginId) const {
    std::shared_ptr<ManagedPlugin> managed;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = plugins_.find(pluginId);
        if (it == plugins_.end()) {
            return nullptr;
        }
        managed = it->second;
    }
    IPlugin* const plugin = managed->plugin.get();
    return std::shared_ptr<IPlugin>(std::move(managed), plugin);
}

IPlugin* PluginManager::getPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = plugins_.find(pluginId);
    return it != plugins_.end() ? it->second->plugin.get() : nullptr;
}

const IPlugin* PluginManager::getPlugin(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = plugins_.find(pluginId);
    return it != plugins_.end() ? it->second->plugin.get() : nullptr;
}

bool PluginManager::suspendPlugin(const std::string& pluginId) {
    std::shared_ptr<IPlugin> plugin;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (shuttingDown_ || unloadingAll_ || transitioning_.count(pluginId) != 0U) {
            return false;
        }
        const auto it = plugins_.find(pluginId);
        if (it == plugins_.end()) {
            return false;
        }
        transitioning_.emplace(pluginId, std::this_thread::get_id());
        plugin = it->second->plugin;
    }

    bool suspended = false;
    try {
        if (plugin->getState() == PluginState::ACTIVE) {
            plugin->setState(PluginState::SUSPENDED);
            suspended = true;
        }
    } catch (...) {
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        finishTransitionLocked(pluginId);
    }
    return suspended;
}

bool PluginManager::resumePlugin(const std::string& pluginId) {
    std::shared_ptr<IPlugin> plugin;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (shuttingDown_ || unloadingAll_ || transitioning_.count(pluginId) != 0U) {
            return false;
        }
        const auto it = plugins_.find(pluginId);
        if (it == plugins_.end()) {
            return false;
        }
        transitioning_.emplace(pluginId, std::this_thread::get_id());
        plugin = it->second->plugin;
    }

    bool resumed = false;
    try {
        if (plugin->getState() == PluginState::SUSPENDED) {
            plugin->setState(PluginState::ACTIVE);
            resumed = true;
        }
    } catch (...) {
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        finishTransitionLocked(pluginId);
    }
    return resumed;
}

std::vector<std::string> PluginManager::getLoadedPluginIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    ids.reserve(plugins_.size());
    for (const auto& entry : plugins_) {
        ids.push_back(entry.first);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

bool PluginManager::checkPluginHealth(const std::string& pluginId) const {
    auto plugin = getPluginShared(pluginId);
    if (!plugin) {
        return false;
    }
    try {
        return plugin->healthCheck();
    } catch (...) {
        return false;
    }
}

void PluginManager::setAgentState(std::shared_ptr<State> state) {
    std::vector<std::shared_ptr<PluginContext>> contexts;
    std::shared_ptr<State> previous;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (shuttingDown_) {
            return;
        }
        previous = std::move(agentState_);
        agentState_ = state;
        contexts.reserve(plugins_.size());
        for (const auto& entry : plugins_) {
            contexts.push_back(entry.second->context);
        }
    }
    for (const auto& context : contexts) {
        context->setAgentState(state);
    }
}

SimpleActionPlugin::SimpleActionPlugin(const std::string& id, const std::string& name,
                                       const std::string& description) {
    metadata_.id = id;
    metadata_.name = name;
    metadata_.description = description;
}

bool SimpleActionPlugin::initialize(const PluginConfig& config, PluginContext& context) {
    std::string error;
    if (!config.validate(error)) {
        state_.store(PluginState::ERROR);
        context.logError("initialization failed: " + error);
        return false;
    }
    config_ = config;
    context_ = &context;
    state_.store(PluginState::INITIALIZED);
    return true;
}

void SimpleActionPlugin::shutdown() {
    context_ = nullptr;
    state_.store(PluginState::UNLOADED);
}

PluginMetadata SimpleActionPlugin::getMetadata() const {
    return metadata_;
}

SimpleProviderPlugin::SimpleProviderPlugin(const std::string& id, const std::string& name,
                                           const std::string& description) {
    metadata_.id = id;
    metadata_.name = name;
    metadata_.description = description;
}

bool SimpleProviderPlugin::initialize(const PluginConfig& config, PluginContext& context) {
    std::string error;
    if (!config.validate(error)) {
        state_.store(PluginState::ERROR);
        context.logError("initialization failed: " + error);
        return false;
    }
    config_ = config;
    context_ = &context;
    state_.store(PluginState::INITIALIZED);
    return true;
}

void SimpleProviderPlugin::shutdown() {
    context_ = nullptr;
    state_.store(PluginState::UNLOADED);
}

PluginMetadata SimpleProviderPlugin::getMetadata() const {
    return metadata_;
}

namespace PluginUtils {

std::string generatePluginId(const std::string& prefix) {
    static std::atomic<std::uint64_t> counter{0U};
    const std::string safePrefix = validIdentifier(prefix) ? prefix : "plugin";
    return safePrefix + "-" + std::to_string(counter.fetch_add(1U) + 1U);
}

bool validateMetadata(const PluginMetadata& metadata, std::string& error) {
    error.clear();
    if (!validIdentifier(metadata.id)) {
        error = "Plugin ID must be 1-128 characters using letters, digits, '.', '_' or '-'";
        return false;
    }
    if (metadata.name.empty() || metadata.name.size() > 256U) {
        error = "Plugin name must be 1-256 characters";
        return false;
    }
    if (metadata.description.size() > 8192U || metadata.author.size() > 512U ||
        !metadata.version.isValid()) {
        error = "Plugin metadata contains invalid sizes or version";
        return false;
    }

    std::set<std::string> tags;
    for (const auto& tag : metadata.tags) {
        if (tag.empty() || tag.size() > 128U || !tags.insert(tag).second) {
            error = "Plugin tag list is invalid";
            return false;
        }
    }

    std::set<std::string> dependencies;
    for (const auto& dependency : metadata.dependencies) {
        if (!validIdentifier(dependency) || dependency == metadata.id ||
            !dependencies.insert(dependency).second) {
            error = "Plugin dependency list is invalid";
            return false;
        }
    }
    return true;
}

bool checkDependencies(const std::vector<std::string>& dependencies,
                       const PluginManager& manager,
                       std::vector<std::string>& missing) {
    missing.clear();
    std::set<std::string> uniqueMissing;
    for (const auto& dependency : dependencies) {
        if (!manager.getPluginShared(dependency)) {
            uniqueMissing.insert(dependency);
        }
    }
    missing.assign(uniqueMissing.begin(), uniqueMissing.end());
    return missing.empty();
}

std::string stateToString(PluginState state) {
    switch (state) {
        case PluginState::UNLOADED:
            return "UNLOADED";
        case PluginState::LOADED:
            return "LOADED";
        case PluginState::INITIALIZED:
            return "INITIALIZED";
        case PluginState::ACTIVE:
            return "ACTIVE";
        case PluginState::SUSPENDED:
            return "SUSPENDED";
        case PluginState::ERROR:
            return "ERROR";
    }
    return "UNKNOWN";
}

std::optional<PluginState> tryStateFromString(const std::string& value) {
    if (value == "UNLOADED") {
        return PluginState::UNLOADED;
    }
    if (value == "LOADED") {
        return PluginState::LOADED;
    }
    if (value == "INITIALIZED") {
        return PluginState::INITIALIZED;
    }
    if (value == "ACTIVE") {
        return PluginState::ACTIVE;
    }
    if (value == "SUSPENDED") {
        return PluginState::SUSPENDED;
    }
    if (value == "ERROR") {
        return PluginState::ERROR;
    }
    return std::nullopt;
}

PluginState stateFromString(const std::string& value) {
    return tryStateFromString(value).value_or(PluginState::UNLOADED);
}

} // namespace PluginUtils
} // namespace elizaos
