#include "elizaos/plugins_automation.hpp"
#include "elizaos/agentlogger.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <future>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <thread>
#include <unordered_set>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace elizaos {
namespace {

namespace fs = std::filesystem;

std::string shellQuote(const std::string& value) {
#ifdef _WIN32
    std::string quoted = "\"";
    for (char ch : value) {
        if (ch == '"') quoted += "\\\"";
        else quoted += ch;
    }
    quoted += "\"";
    return quoted;
#else
    std::string quoted = "'";
    for (char ch : value) {
        if (ch == '\'') quoted += "'\\''";
        else quoted += ch;
    }
    quoted += "'";
    return quoted;
#endif
}

void replaceAll(std::string& value, const std::string& token, const std::string& replacement) {
    if (token.empty()) return;
    std::size_t position = 0;
    while ((position = value.find(token, position)) != std::string::npos) {
        value.replace(position, token.size(), replacement);
        position += replacement.size();
    }
}

std::string expandCommand(std::string command,
                          const std::string& plugin,
                          const std::string& target = {}) {
    const bool hadPluginToken = command.find("{plugin}") != std::string::npos;
    const bool hadTargetToken = command.find("{target}") != std::string::npos;
    replaceAll(command, "{plugin}", shellQuote(plugin));
    replaceAll(command, "{target}", shellQuote(target));

    std::error_code ec;
    if (!hadPluginToken && fs::is_directory(plugin, ec)) {
        command = "cd " + shellQuote(plugin) + " && " + command;
    }
    if (!hadTargetToken && !target.empty()) {
        command += " " + shellQuote(target);
    }
    return command;
}

std::string cppIdentifier(const std::string& value) {
    std::string result;
    result.reserve(value.size() + 1);
    for (char ch : value) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '_') {
            result += ch;
        } else {
            result += '_';
        }
    }
    if (result.empty()) result = "ElizaPlugin";
    if (result.front() >= '0' && result.front() <= '9') result.insert(result.begin(), '_');
    return result;
}

bool writeTextFile(const fs::path& path, const std::string& content) {
    std::error_code ec;
    if (path.has_parent_path()) fs::create_directories(path.parent_path(), ec);
    if (ec) return false;

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) return false;
    output << content;
    return output.good();
}

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}


bool parseVersion(const std::string& value, int& major, int& minor, int& patch) {
    major = minor = patch = 0;
    if (value.empty()) return false;
    const auto suffix = value.find_first_of("-+");
    const std::string core = value.substr(0, suffix);
    std::istringstream stream(core);
    std::string component;
    int* outputs[] = {&major, &minor, &patch};
    int count = 0;
    while (std::getline(stream, component, '.')) {
        if (count == 3 || component.empty() ||
            !std::all_of(component.begin(), component.end(), [](unsigned char ch) {
                return ch >= '0' && ch <= '9';
            })) return false;
        try {
            std::size_t consumed = 0;
            const int parsed = std::stoi(component, &consumed);
            if (consumed != component.size() || parsed < 0) return false;
            *outputs[count++] = parsed;
        } catch (...) {
            return false;
        }
    }
    return count >= 2;
}

bool validMetadata(const PluginMetadata& metadata) {
    int major = 0, minor = 0, patch = 0;
    if (metadata.name.empty() || !parseVersion(metadata.version, major, minor, patch) ||
        (major == 0 && minor == 0 && patch == 0)) return false;
    if (!parseVersion(metadata.apiVersion, major, minor, patch) || major != 1) return false;
    std::unordered_set<std::string> seen;
    for (const auto& dependency : metadata.dependencies) {
        if (dependency.empty() || dependency == metadata.name || !seen.insert(dependency).second) {
            return false;
        }
    }
    return true;
}

bool sameDependencies(std::vector<std::string> lhs, std::vector<std::string> rhs) {
    std::sort(lhs.begin(), lhs.end());
    std::sort(rhs.begin(), rhs.end());
    return lhs == rhs;
}


bool restorePluginState(const std::shared_ptr<Plugin>& plugin,
                        const PluginMetadata& metadata,
                        PluginStatus target) {
    try {
        PluginStatus current = plugin->getStatus();
        if (current == target) return true;
        if (target == PluginStatus::ACTIVE &&
            (current == PluginStatus::LOADED || current == PluginStatus::INACTIVE)) {
            return plugin->activate() && plugin->getStatus() == PluginStatus::ACTIVE;
        }
        if (target == PluginStatus::INACTIVE && current == PluginStatus::ACTIVE) {
            return plugin->deactivate() && plugin->getStatus() == PluginStatus::INACTIVE;
        }
        if (target == PluginStatus::LOADED && current == PluginStatus::ACTIVE) {
            if (!plugin->deactivate() || plugin->getStatus() != PluginStatus::INACTIVE) return false;
            current = PluginStatus::INACTIVE;
        }
        if (current != PluginStatus::UNKNOWN &&
            (!plugin->shutdown() || plugin->getStatus() != PluginStatus::UNKNOWN)) return false;
        if (target == PluginStatus::UNKNOWN) return true;
        if (!plugin->initialize(metadata) || plugin->getStatus() != PluginStatus::LOADED) return false;
        if (target == PluginStatus::LOADED) return true;
        if (!plugin->activate() || plugin->getStatus() != PluginStatus::ACTIVE) return false;
        if (target == PluginStatus::ACTIVE) return true;
        return target == PluginStatus::INACTIVE && plugin->deactivate() &&
               plugin->getStatus() == PluginStatus::INACTIVE;
    } catch (...) {
        return false;
    }
}

PluginStatus reportedStatus(const std::shared_ptr<Plugin>& plugin) {
    try { return plugin->getStatus(); } catch (...) { return PluginStatus::FAILED; }
}

} // namespace

std::shared_ptr<PluginsAutomation> globalPluginAutomation = std::make_shared<PluginsAutomation>();

PluginRegistry::PluginRegistry() = default;

PluginRegistry::~PluginRegistry() {
    std::vector<std::pair<std::string, std::shared_ptr<Plugin>>> plugins;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        plugins.reserve(plugins_.size());
        for (auto& entry : plugins_) plugins.emplace_back(entry.first, entry.second);
        plugins_.clear();
        metadata_.clear();
        states_.clear();
        stableStates_.clear();
        transitions_.clear();
    }
    std::sort(plugins.begin(), plugins.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.first > rhs.first;
    });
    for (auto& entry : plugins) {
        auto& plugin = entry.second;
        if (!plugin) continue;
        try {
            if (plugin->getStatus() == PluginStatus::ACTIVE) plugin->deactivate();
        } catch (...) {}
        try { plugin->shutdown(); } catch (...) {}
    }
}

bool PluginRegistry::registerPlugin(std::shared_ptr<Plugin> plugin,
                                    const PluginMetadata& metadata) {
    if (!plugin || !validMetadata(metadata)) return false;

    std::string actualName;
    std::string actualVersion;
    std::vector<std::string> actualDependencies;
    PluginStatus initialStatus = PluginStatus::FAILED;
    try {
        actualName = plugin->getName();
        actualVersion = plugin->getVersion();
        actualDependencies = plugin->getDependencies();
        initialStatus = plugin->getStatus();
    } catch (...) {
        return false;
    }
    if (actualName != metadata.name || actualVersion != metadata.version ||
        !sameDependencies(actualDependencies, metadata.dependencies) ||
        initialStatus != PluginStatus::UNKNOWN) return false;

    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        if (plugins_.count(metadata.name) != 0 || transitions_.count(metadata.name) != 0 ||
            !checkDependencies(metadata)) return false;
        plugins_[metadata.name] = plugin;
        metadata_[metadata.name] = metadata;
        states_[metadata.name] = PluginStatus::LOADING;
        stableStates_[metadata.name] = PluginStatus::UNKNOWN;
        transitions_[metadata.name] = true;
    }

    bool initialized = false;
    PluginStatus reported = PluginStatus::FAILED;
    try {
        initialized = plugin->initialize(metadata);
        reported = plugin->getStatus();
    } catch (...) {
        initialized = false;
    }

    if (!initialized || reported != PluginStatus::LOADED) {
        try { plugin->shutdown(); } catch (...) {}
        std::lock_guard<std::mutex> lock(registryMutex_);
        plugins_.erase(metadata.name);
        metadata_.erase(metadata.name);
        states_.erase(metadata.name);
        stableStates_.erase(metadata.name);
        transitions_.erase(metadata.name);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        states_[metadata.name] = PluginStatus::LOADED;
        stableStates_[metadata.name] = PluginStatus::LOADED;
        transitions_.erase(metadata.name);
    }
    logInfo("Registered and initialized plugin: " + metadata.name + " v" + metadata.version,
            "plugins_automation");
    return true;
}

bool PluginRegistry::initializePlugin(const std::string& name) {
    std::shared_ptr<Plugin> plugin;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        auto it = plugins_.find(name);
        if (it == plugins_.end() || transitions_.count(name) != 0) return false;
        auto state = states_.find(name);
        if (state != states_.end() && state->second == PluginStatus::LOADED) {
            plugin = it->second;
        } else {
            return false;
        }
    }
    try {
        return plugin && plugin->getStatus() == PluginStatus::LOADED && plugin->healthCheck();
    } catch (...) {
        return false;
    }
}

bool PluginRegistry::activatePlugin(const std::string& name) {
    std::shared_ptr<Plugin> plugin;
    PluginMetadata metadata;
    PluginStatus previous = PluginStatus::UNKNOWN;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        auto it = plugins_.find(name);
        if (it == plugins_.end() || transitions_.count(name) != 0) return false;
        previous = states_[name];
        if (previous != PluginStatus::LOADED && previous != PluginStatus::INACTIVE) return false;
        const auto& dependencies = metadata_.at(name).dependencies;
        for (const auto& dependency : dependencies) {
            auto state = states_.find(dependency);
            if (state == states_.end() || state->second != PluginStatus::ACTIVE) return false;
        }
        plugin = it->second;
        metadata = metadata_.at(name);
        stableStates_[name] = previous;
        transitions_[name] = true;
    }

    bool activated = false;
    PluginStatus reported = PluginStatus::FAILED;
    try {
        activated = plugin->activate();
        reported = plugin->getStatus();
    } catch (...) {
        activated = false;
    }
    if (!activated || reported != PluginStatus::ACTIVE) {
        const bool rolledBack = restorePluginState(plugin, metadata, previous);
        std::lock_guard<std::mutex> lock(registryMutex_);
        states_[name] = rolledBack ? previous : reportedStatus(plugin);
        stableStates_[name] = states_[name];
        transitions_.erase(name);
        return false;
    }

    std::lock_guard<std::mutex> lock(registryMutex_);
    states_[name] = PluginStatus::ACTIVE;
    stableStates_[name] = PluginStatus::ACTIVE;
    transitions_.erase(name);
    return true;
}

bool PluginRegistry::deactivatePlugin(const std::string& name) {
    std::shared_ptr<Plugin> plugin;
    PluginMetadata metadata;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        auto it = plugins_.find(name);
        if (it == plugins_.end() || transitions_.count(name) != 0 ||
            states_[name] != PluginStatus::ACTIVE) return false;
        plugin = it->second;
        metadata = metadata_.at(name);
        stableStates_[name] = PluginStatus::ACTIVE;
        transitions_[name] = true;
    }

    bool deactivated = false;
    PluginStatus reported = PluginStatus::FAILED;
    try {
        deactivated = plugin->deactivate();
        reported = plugin->getStatus();
    } catch (...) {
        deactivated = false;
    }
    if (!deactivated || reported != PluginStatus::INACTIVE) {
        const bool rolledBack = restorePluginState(plugin, metadata, PluginStatus::ACTIVE);
        std::lock_guard<std::mutex> lock(registryMutex_);
        states_[name] = rolledBack ? PluginStatus::ACTIVE : reportedStatus(plugin);
        stableStates_[name] = states_[name];
        transitions_.erase(name);
        return false;
    }

    std::lock_guard<std::mutex> lock(registryMutex_);
    states_[name] = PluginStatus::INACTIVE;
    stableStates_[name] = PluginStatus::INACTIVE;
    transitions_.erase(name);
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& name) {
    std::shared_ptr<Plugin> plugin;
    PluginMetadata metadata;
    PluginStatus previous = PluginStatus::UNKNOWN;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        auto it = plugins_.find(name);
        if (it == plugins_.end() || transitions_.count(name) != 0 || hasDependents(name)) {
            return false;
        }
        plugin = it->second;
        metadata = metadata_.at(name);
        previous = states_[name];
        states_[name] = PluginStatus::UNLOADING;
        stableStates_[name] = previous;
        transitions_[name] = true;
    }

    bool deactivated = previous != PluginStatus::ACTIVE;
    if (!deactivated) {
        try { deactivated = plugin->deactivate() && plugin->getStatus() == PluginStatus::INACTIVE; }
        catch (...) { deactivated = false; }
    }
    if (!deactivated) {
        std::lock_guard<std::mutex> lock(registryMutex_);
        states_[name] = previous;
        stableStates_[name] = previous;
        transitions_.erase(name);
        return false;
    }

    bool shutdown = false;
    PluginStatus reported = PluginStatus::FAILED;
    try {
        shutdown = plugin->shutdown();
        reported = plugin->getStatus();
    } catch (...) {
        shutdown = false;
    }
    if (!shutdown || reported != PluginStatus::UNKNOWN) {
        const bool rolledBack = restorePluginState(plugin, metadata, previous);
        std::lock_guard<std::mutex> lock(registryMutex_);
        states_[name] = rolledBack ? previous : reportedStatus(plugin);
        stableStates_[name] = states_[name];
        transitions_.erase(name);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        plugins_.erase(name);
        metadata_.erase(name);
        states_.erase(name);
        stableStates_.erase(name);
        transitions_.erase(name);
    }
    logInfo("Unregistered plugin: " + name, "plugins_automation");
    return true;
}

std::shared_ptr<Plugin> PluginRegistry::getPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = plugins_.find(name);
    return it != plugins_.end() ? it->second : nullptr;
}

std::vector<std::string> PluginRegistry::getPluginNames() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    for (const auto& entry : plugins_) names.push_back(entry.first);
    std::sort(names.begin(), names.end());
    return names;
}

PluginStatus PluginRegistry::getPluginStatus(const std::string& name) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = states_.find(name);
    return it != states_.end() ? it->second : PluginStatus::UNKNOWN;
}

bool PluginRegistry::checkPluginHealth(const std::string& name) const {
    std::shared_ptr<Plugin> plugin;
    PluginStatus expected = PluginStatus::UNKNOWN;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        auto it = plugins_.find(name);
        auto state = states_.find(name);
        if (it == plugins_.end() || state == states_.end() || transitions_.count(name) != 0) {
            return false;
        }
        expected = state->second;
        if (expected != PluginStatus::LOADED && expected != PluginStatus::ACTIVE &&
            expected != PluginStatus::INACTIVE) return false;
        plugin = it->second;
    }
    try {
        return plugin->getStatus() == expected && plugin->healthCheck();
    } catch (...) {
        return false;
    }
}

std::vector<std::string> PluginRegistry::getActivePlugins() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    std::vector<std::string> active;
    for (const auto& entry : states_) {
        if (entry.second == PluginStatus::ACTIVE) active.push_back(entry.first);
    }
    std::sort(active.begin(), active.end());
    return active;
}

std::vector<std::string> PluginRegistry::getFailedPlugins() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    std::vector<std::string> failed;
    for (const auto& entry : states_) {
        if (entry.second == PluginStatus::FAILED) failed.push_back(entry.first);
    }
    std::sort(failed.begin(), failed.end());
    return failed;
}

bool PluginRegistry::resolveDependencies(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto metaIt = metadata_.find(pluginName);
    if (metaIt == metadata_.end()) return false;
    for (const auto& dependency : metaIt->second.dependencies) {
        auto state = states_.find(dependency);
        if (state == states_.end() || state->second != PluginStatus::ACTIVE) return false;
    }
    return true;
}

std::vector<std::string> PluginRegistry::getDependencyChain(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = metadata_.find(pluginName);
    return it != metadata_.end() ? it->second.dependencies : std::vector<std::string>{};
}

bool PluginRegistry::checkDependencies(const PluginMetadata& metadata) const {
    for (const auto& dependency : metadata.dependencies) {
        auto plugin = plugins_.find(dependency);
        auto state = states_.find(dependency);
        if (plugin == plugins_.end() || state == states_.end() ||
            (state->second != PluginStatus::LOADED && state->second != PluginStatus::ACTIVE &&
             state->second != PluginStatus::INACTIVE)) return false;
    }
    return true;
}

bool PluginRegistry::hasDependents(const std::string& name) const {
    for (const auto& entry : metadata_) {
        if (entry.first == name) continue;
        if (std::find(entry.second.dependencies.begin(), entry.second.dependencies.end(), name) !=
            entry.second.dependencies.end()) return true;
    }
    return false;
}

CIPipeline::CIPipeline()
    : buildCommand_("cmake -S . -B build && cmake --build build"),
      testCommand_("ctest --test-dir build --output-on-failure"),
      deployCommand_("cmake --install build --prefix") {}

CIPipeline::~CIPipeline() = default;

std::future<bool> CIPipeline::buildPlugin(const std::string& pluginPath) {
    const std::string command = expandCommand(buildCommand_, pluginPath);
    return std::async(std::launch::async, [this, pluginPath, command]() {
        return runStage(pluginPath, "build", command);
    });
}

std::future<bool> CIPipeline::testPlugin(const std::string& pluginName) {
    const std::string command = expandCommand(testCommand_, pluginName);
    return std::async(std::launch::async, [this, pluginName, command]() {
        return runStage(pluginName, "test", command);
    });
}

std::future<bool> CIPipeline::deployPlugin(const std::string& pluginName,
                                            const std::string& target) {
    const std::string command = expandCommand(deployCommand_, pluginName, target);
    return std::async(std::launch::async, [this, pluginName, command]() {
        return runStage(pluginName, "deploy", command);
    });
}

void CIPipeline::setBuildCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    buildCommand_ = command;
}

void CIPipeline::setTestCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    testCommand_ = command;
}

void CIPipeline::setDeployCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    deployCommand_ = command;
}

CIPipeline::PipelineStatus CIPipeline::getStatus(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    auto it = statuses_.find(pluginName);
    return it != statuses_.end()
        ? it->second
        : PipelineStatus{pluginName, "unknown", false, false, {}, {}};
}

std::vector<CIPipeline::PipelineStatus> CIPipeline::getAllStatuses() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    std::vector<PipelineStatus> statuses;
    statuses.reserve(statuses_.size());
    for (const auto& [plugin, status] : statuses_) {
        (void)plugin;
        statuses.push_back(status);
    }
    std::sort(statuses.begin(), statuses.end(),
              [](const PipelineStatus& lhs, const PipelineStatus& rhs) {
                  return lhs.pluginName < rhs.pluginName;
              });
    return statuses;
}

bool CIPipeline::executeCommand(const std::string& command,
                                std::string& output,
                                std::string& error) {
    output.clear();
    error.clear();
    if (trim(command).empty()) {
        error = "Pipeline command is empty";
        return false;
    }

    const std::string redirected = command + " 2>&1";
#ifdef _WIN32
    FILE* pipe = _popen(redirected.c_str(), "r");
#else
    FILE* pipe = popen(redirected.c_str(), "r");
#endif
    if (!pipe) {
        error = "Unable to start pipeline command";
        return false;
    }

    std::array<char, 4096> buffer{};
    while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe)) {
        output.append(buffer.data());
    }

#ifdef _WIN32
    const int status = _pclose(pipe);
    const bool success = status == 0;
#else
    const int status = pclose(pipe);
    const bool success = status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0;
#endif
    if (!success) {
        error = output.empty() ? "Pipeline command failed" : output;
    }
    return success;
}

bool CIPipeline::runStage(const std::string& pluginName,
                          const std::string& stage,
                          const std::string& command) {
    {
        std::lock_guard<std::mutex> lock(statusMutex_);
        statuses_[pluginName] = PipelineStatus{pluginName, stage, true, false, {}, {}};
    }

    std::string output;
    std::string error;
    const bool success = executeCommand(command, output, error);

    {
        std::lock_guard<std::mutex> lock(statusMutex_);
        statuses_[pluginName] = PipelineStatus{pluginName, stage, false, success,
                                               std::move(output), std::move(error)};
    }
    return success;
}

struct PluginTester::ExecutorState {
    struct Job {
        CooperativeTestFunction function;
        std::shared_ptr<std::atomic<bool>> stopRequested;
        std::promise<bool> completion;
    };

    std::mutex mutex;
    std::condition_variable condition;
    std::deque<std::shared_ptr<Job>> queue;
    std::shared_ptr<std::atomic<bool>> activeStop;
    std::size_t outstanding = 0;
    bool accepting = true;
    std::thread worker;
};

PluginTester::StopToken::StopToken(std::shared_ptr<std::atomic<bool>> stopRequested)
    : stopRequested_(std::move(stopRequested)) {}

bool PluginTester::StopToken::stopRequested() const noexcept {
    return stopRequested_ && stopRequested_->load(std::memory_order_acquire);
}

PluginTester::PluginTester()
    : timeoutSeconds_(30), verbose_(false), maxPendingTests_(1),
      executor_(std::make_shared<ExecutorState>()) {
    const auto state = executor_;
    state->worker = std::thread([state] {
        for (;;) {
            std::shared_ptr<ExecutorState::Job> job;
            {
                std::unique_lock<std::mutex> lock(state->mutex);
                state->condition.wait(lock, [&] {
                    return !state->accepting || !state->queue.empty();
                });
                if (state->queue.empty()) {
                    if (!state->accepting) break;
                    continue;
                }
                job = std::move(state->queue.front());
                state->queue.pop_front();
                state->activeStop = job->stopRequested;
            }
            try {
                const bool value = job->function(StopToken(job->stopRequested));
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    state->activeStop.reset();
                    if (state->outstanding > 0) --state->outstanding;
                }
                job->completion.set_value(value);
            } catch (...) {
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    state->activeStop.reset();
                    if (state->outstanding > 0) --state->outstanding;
                }
                try { job->completion.set_exception(std::current_exception()); } catch (...) {}
            }
            state->condition.notify_all();
        }
    });
}

PluginTester::~PluginTester() {
    auto state = executor_;
    if (!state) return;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->accepting = false;
        if (state->activeStop) state->activeStop->store(true, std::memory_order_release);
        for (const auto& job : state->queue) {
            job->stopRequested->store(true, std::memory_order_release);
        }
    }
    state->condition.notify_all();
    if (state->worker.joinable()) state->worker.join();
}

void PluginTester::addTestCase(const std::string& testName, std::function<bool()> testFunc) {
    if (!testFunc) return;
    addTestCase(testName, [test = std::move(testFunc)](const StopToken&) { return test(); });
}

void PluginTester::addTestCase(const std::string& testName,
                               CooperativeTestFunction testFunc) {
    if (testName.empty() || !testFunc) return;
    std::lock_guard<std::mutex> lock(testMutex_);
    testCases_[testName] = std::move(testFunc);
}

void PluginTester::removeTestCase(const std::string& testName) {
    std::lock_guard<std::mutex> lock(testMutex_);
    testCases_.erase(testName);
}

std::vector<PluginTester::TestResult> PluginTester::runTests(const std::string& pluginName) {
    std::vector<std::pair<std::string, CooperativeTestFunction>> selected;
    {
        std::lock_guard<std::mutex> lock(testMutex_);
        for (const auto& entry : testCases_) {
            const auto& name = entry.first;
            if (name == pluginName || name.rfind(pluginName + "::", 0) == 0 ||
                name.rfind(pluginName + ".", 0) == 0) selected.emplace_back(entry);
        }
    }
    std::sort(selected.begin(), selected.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.first < rhs.first;
    });
    std::vector<TestResult> results;
    results.reserve(selected.size());
    for (auto& entry : selected) results.push_back(executeTest(entry.first, std::move(entry.second)));
    return results;
}

std::vector<PluginTester::TestResult> PluginTester::runAllTests() {
    std::vector<std::pair<std::string, CooperativeTestFunction>> tests;
    {
        std::lock_guard<std::mutex> lock(testMutex_);
        tests.reserve(testCases_.size());
        for (const auto& entry : testCases_) tests.emplace_back(entry);
    }
    std::sort(tests.begin(), tests.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.first < rhs.first;
    });
    std::vector<TestResult> results;
    results.reserve(tests.size());
    for (auto& entry : tests) results.push_back(executeTest(entry.first, std::move(entry.second)));
    return results;
}

void PluginTester::setTimeout(int seconds) {
    std::lock_guard<std::mutex> lock(testMutex_);
    timeoutSeconds_ = std::max(0, seconds);
}

void PluginTester::setVerbose(bool verbose) {
    std::lock_guard<std::mutex> lock(testMutex_);
    verbose_ = verbose;
}

void PluginTester::setMaxPendingTests(std::size_t maxPendingTests) {
    std::lock_guard<std::mutex> lock(testMutex_);
    maxPendingTests_ = std::max<std::size_t>(1, maxPendingTests);
}

PluginTester::TestResult PluginTester::executeTest(const std::string& testName,
                                                    CooperativeTestFunction testFunc) {
    const auto started = std::chrono::steady_clock::now();
    int timeout = 0;
    bool verbose = false;
    std::size_t maxPending = 1;
    {
        std::lock_guard<std::mutex> lock(testMutex_);
        timeout = timeoutSeconds_;
        verbose = verbose_;
        maxPending = maxPendingTests_;
    }

    auto job = std::make_shared<ExecutorState::Job>();
    job->function = std::move(testFunc);
    job->stopRequested = std::make_shared<std::atomic<bool>>(false);
    auto future = job->completion.get_future();
    {
        std::lock_guard<std::mutex> lock(executor_->mutex);
        if (!executor_->accepting || executor_->outstanding >= maxPending) {
            TestResult result(testName, false, "executor admission rejected: capacity reached", 0.0);
            result.admissionRejected = true;
            return result;
        }
        ++executor_->outstanding;
        executor_->queue.push_back(job);
    }
    executor_->condition.notify_one();

    TestResult result(testName, false);
    const auto waitResult = future.wait_for(std::chrono::seconds(timeout));
    if (waitResult == std::future_status::ready) {
        try {
            result.passed = future.get();
            result.message = result.passed ? "passed" : "returned false";
        } catch (const std::exception& ex) {
            result.message = std::string("exception: ") + ex.what();
        } catch (...) {
            result.message = "unknown exception";
        }
    } else {
        job->stopRequested->store(true, std::memory_order_release);
        result.timedOut = true;
        result.message = "timed out after " + std::to_string(timeout) +
                         " seconds; cooperative stop requested";
    }

    result.executionTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - started).count();
    if (verbose) logInfo("Plugin test " + testName + ": " + result.message,
                         "plugins_automation");
    return result;
}

WorkflowAutomation::WorkflowAutomation() : templateDirectory_("templates") {}
WorkflowAutomation::~WorkflowAutomation() = default;

bool WorkflowAutomation::createPluginTemplate(const std::string& pluginName,
                                                const std::string& outputPath) {
    if (pluginName.empty() || outputPath.empty()) return false;
    const fs::path destination(outputPath);
    std::error_code ec;
    if (fs::exists(destination, ec) && !fs::is_empty(destination, ec)) return false;

    const fs::path configuredTemplate(templateDirectory_);
    if (fs::is_directory(configuredTemplate, ec) && !ec &&
        fs::directory_iterator(configuredTemplate, ec) != fs::directory_iterator()) {
        if (!copyTemplate(configuredTemplate.string(), destination.string())) return false;
        return replaceTokens((destination / "plugin.json").string(),
                             {{"PLUGIN_NAME", pluginName},
                              {"PLUGIN_CLASS", cppIdentifier(pluginName)}});
    }

    fs::create_directories(destination / "include", ec);
    if (ec) return false;
    fs::create_directories(destination / "src", ec);
    if (ec) return false;
    fs::create_directories(destination / "tests", ec);
    if (ec) return false;

    const std::string identifier = cppIdentifier(pluginName);
    const std::string header =
        "#pragma once\n\n#include <string>\n\nclass " + identifier +
        " {\npublic:\n    bool initialize();\n    std::string name() const;\n};\n";
    const std::string source =
        "#include \"" + pluginName + ".hpp\"\n\nbool " + identifier +
        "::initialize() { return true; }\nstd::string " + identifier +
        "::name() const { return \"" + pluginName + "\"; }\n";
    const std::string test =
        "#include \"" + pluginName +
        ".hpp\"\n\nint main() { " + identifier +
        " plugin; return plugin.initialize() && plugin.name() == \"" + pluginName +
        "\" ? 0 : 1; }\n";
    const std::string cmake =
        "cmake_minimum_required(VERSION 3.16)\nproject(" + identifier +
        " LANGUAGES CXX)\nset(CMAKE_CXX_STANDARD 17)\ninclude(CTest)\nadd_library(" + identifier +
        " src/" + pluginName + ".cpp)\ntarget_include_directories(" + identifier +
        " PUBLIC include)\nadd_executable(" + identifier + "_test tests/test_" + pluginName +
        ".cpp)\ntarget_link_libraries(" + identifier + "_test PRIVATE " + identifier +
        ")\nadd_test(NAME " + identifier + "_test COMMAND " + identifier +
        "_test)\ninstall(TARGETS " + identifier +
        " ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)\n"
        "install(DIRECTORY include/ DESTINATION include)\n"
        "install(FILES plugin.json DESTINATION share/" + pluginName + ")\n";
    const std::string metadata =
        "{\n  \"name\": \"" + pluginName +
        "\",\n  \"version\": \"0.1.0\",\n  \"entry\": \"src/" + pluginName +
        ".cpp\"\n}\n";

    return writeTextFile(destination / "include" / (pluginName + ".hpp"), header) &&
           writeTextFile(destination / "src" / (pluginName + ".cpp"), source) &&
           writeTextFile(destination / "tests" / ("test_" + pluginName + ".cpp"), test) &&
           writeTextFile(destination / "CMakeLists.txt", cmake) &&
           writeTextFile(destination / "plugin.json", metadata);
}

bool WorkflowAutomation::generatePluginDocs(const std::string& pluginPath) {
    const fs::path root(pluginPath);
    if (!validatePluginStructure(pluginPath)) return false;

    std::vector<std::string> files;
    std::error_code ec;
    for (fs::recursive_directory_iterator it(root, ec), end; it != end && !ec; it.increment(ec)) {
        if (it->is_regular_file()) files.push_back(it->path().lexically_relative(root).generic_string());
    }
    if (ec) return false;
    std::sort(files.begin(), files.end());

    std::ostringstream readme;
    readme << "# " << root.filename().string() << "\n\n"
           << "Generated ElizaOS C++ plugin scaffold.\n\n## Files\n\n";
    for (const auto& file : files) readme << "- `" << file << "`\n";
    return writeTextFile(root / "README.md", readme.str());
}

bool WorkflowAutomation::validatePluginStructure(const std::string& pluginPath) {
    const fs::path root(pluginPath);
    std::error_code ec;
    if (!fs::is_directory(root, ec) || ec) return false;
    if (!fs::is_regular_file(root / "CMakeLists.txt", ec) || ec) return false;
    if (!fs::is_regular_file(root / "plugin.json", ec) || ec) return false;
    if (!fs::is_directory(root / "include", ec) || ec) return false;
    if (!fs::is_directory(root / "src", ec) || ec) return false;
    if (!fs::is_directory(root / "tests", ec) || ec) return false;

    auto hasSource = [](const fs::path& directory) {
        std::error_code localError;
        for (fs::directory_iterator it(directory, localError), end;
             it != end && !localError; it.increment(localError)) {
            const auto extension = it->path().extension().string();
            if (it->is_regular_file() &&
                (extension == ".cpp" || extension == ".cc" || extension == ".cxx")) return true;
        }
        return false;
    };
    auto hasHeader = [](const fs::path& directory) {
        std::error_code localError;
        for (fs::directory_iterator it(directory, localError), end;
             it != end && !localError; it.increment(localError)) {
            const auto extension = it->path().extension().string();
            if (it->is_regular_file() &&
                (extension == ".hpp" || extension == ".h" || extension == ".hxx")) return true;
        }
        return false;
    };
    return hasSource(root / "src") && hasHeader(root / "include") && hasSource(root / "tests");
}

bool WorkflowAutomation::packagePlugin(const std::string& pluginPath,
                                        const std::string& outputPath) {
    if (!validatePluginStructure(pluginPath) || outputPath.empty()) return false;
    const fs::path source(pluginPath);
    const fs::path destination(outputPath);
    std::error_code ec;
    if (fs::exists(destination, ec)) return false;

    fs::create_directories(destination.parent_path(), ec);
    if (ec) return false;
    fs::copy(source, destination,
             fs::copy_options::recursive | fs::copy_options::copy_symlinks, ec);
    if (ec) return false;

    std::ostringstream manifest;
    manifest << "source=" << fs::absolute(source, ec).generic_string() << "\n";
    manifest << "packaged_at_epoch_ms="
             << std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()
             << "\n";
    return writeTextFile(destination / "PACKAGE_MANIFEST.txt", manifest.str());
}

bool WorkflowAutomation::generateInterface(const std::string& interfaceName,
                                             const std::string& outputPath) {
    if (interfaceName.empty() || outputPath.empty()) return false;
    fs::path destination(outputPath);
    if (!destination.has_extension()) destination /= interfaceName + ".hpp";
    const std::string identifier = cppIdentifier(interfaceName);
    return writeTextFile(destination,
        "#pragma once\n\n#include <string>\n\nclass I" + identifier +
        " {\npublic:\n    virtual ~I" + identifier +
        "() = default;\n    virtual bool initialize() = 0;\n"
        "    virtual void shutdown() = 0;\n    virtual std::string name() const = 0;\n};\n");
}

bool WorkflowAutomation::generateTestSkeleton(const std::string& pluginName,
                                                const std::string& outputPath) {
    if (pluginName.empty() || outputPath.empty()) return false;
    fs::path destination(outputPath);
    if (!destination.has_extension()) destination /= "test_" + pluginName + ".cpp";
    const std::string identifier = cppIdentifier(pluginName);
    return writeTextFile(destination,
        "#include <gtest/gtest.h>\n#include \"" + pluginName +
        ".hpp\"\n\nTEST(" + identifier +
        "Test, PublicContract) {\n    " + identifier +
        " plugin;\n    EXPECT_TRUE(plugin.initialize());\n}\n");
}

void WorkflowAutomation::setTemplateDirectory(const std::string& path) {
    templateDirectory_ = path;
}

std::vector<std::string> WorkflowAutomation::getAvailableTemplates() const {
    std::vector<std::string> templates;
    std::error_code ec;
    const fs::path root(templateDirectory_);
    if (!fs::is_directory(root, ec) || ec) return templates;
    for (fs::directory_iterator it(root, ec), end; it != end && !ec; it.increment(ec)) {
        if (it->is_directory()) templates.push_back(it->path().filename().string());
    }
    std::sort(templates.begin(), templates.end());
    return templates;
}

bool WorkflowAutomation::copyTemplate(const std::string& templateName,
                                      const std::string& destination) {
    const fs::path source(templateName);
    const fs::path target(destination);
    std::error_code ec;
    if (!fs::is_directory(source, ec) || ec) return false;
    fs::create_directories(target, ec);
    if (ec) return false;
    fs::copy(source, target,
             fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
    return !ec;
}

bool WorkflowAutomation::replaceTokens(
    const std::string& filePath,
    const std::unordered_map<std::string, std::string>& tokens) {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) return tokens.empty();
    std::ostringstream buffer;
    buffer << input.rdbuf();
    std::string content = buffer.str();
    for (const auto& [key, value] : tokens) {
        replaceAll(content, "{{" + key + "}}", value);
    }
    return writeTextFile(filePath, content);
}

PluginsAutomation::PluginsAutomation() = default;
PluginsAutomation::~PluginsAutomation() = default;

bool PluginsAutomation::automatedPluginSetup(const std::string& pluginName,
                                              const std::string& templateName) {
    if (pluginName.empty()) return false;
    std::string outputRoot;
    std::string templateRoot;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        auto output = config_.find("plugin_output_root");
        outputRoot = output != config_.end() ? output->second : ".";
        auto templ = config_.find("template_directory");
        templateRoot = templ != config_.end() ? templ->second : "templates";
    }

    const fs::path destination = fs::path(outputRoot) / pluginName;
    workflow_.setTemplateDirectory(templateName.empty()
        ? templateRoot
        : (fs::path(templateRoot) / templateName).string());
    const bool created = workflow_.createPluginTemplate(pluginName, destination.string());
    workflow_.setTemplateDirectory(templateRoot);
    if (!created) return false;
    return workflow_.generatePluginDocs(destination.string());
}

bool PluginsAutomation::automatedBuildAndTest(const std::string& pluginPath) {
    if (pluginPath.empty() || !workflow_.validatePluginStructure(pluginPath)) return false;
    if (!pipeline_.buildPlugin(pluginPath).get()) return false;
    if (!pipeline_.testPlugin(pluginPath).get()) return false;

    const auto results = tester_.runTests(fs::path(pluginPath).filename().string());
    return std::all_of(results.begin(), results.end(),
                       [](const PluginTester::TestResult& result) { return result.passed; });
}

bool PluginsAutomation::automatedDeployment(const std::string& pluginName,
                                             const std::string& target) {
    if (pluginName.empty() || target.empty()) return false;
    return pipeline_.deployPlugin(pluginName, target).get();
}

void PluginsAutomation::loadConfiguration(const std::string& configPath) {
    std::ifstream input(configPath);
    if (!input) {
        logWarning("Unable to load plugin automation configuration: " + configPath,
                   "plugins_automation");
        return;
    }

    std::unordered_map<std::string, std::string> loaded;
    std::string line;
    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty() || line.front() == '#') continue;
        const auto separator = line.find('=');
        if (separator == std::string::npos) continue;
        const std::string key = trim(line.substr(0, separator));
        const std::string value = trim(line.substr(separator + 1));
        if (!key.empty()) loaded[key] = value;
    }

    std::lock_guard<std::mutex> lock(configMutex_);
    config_ = std::move(loaded);
}

void PluginsAutomation::saveConfiguration(const std::string& configPath) {
    std::vector<std::pair<std::string, std::string>> values;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        values.assign(config_.begin(), config_.end());
    }
    std::sort(values.begin(), values.end());

    const fs::path destination(configPath);
    const fs::path temporary = destination.string() + ".tmp";
    std::ostringstream content;
    content << "# ElizaOS plugin automation configuration\n";
    for (const auto& [key, value] : values) content << key << '=' << value << '\n';
    if (!writeTextFile(temporary, content.str())) {
        logError("Unable to save plugin automation configuration: " + configPath,
                 "plugins_automation");
        return;
    }

    std::error_code ec;
    fs::rename(temporary, destination, ec);
    if (ec) {
        fs::remove(destination, ec);
        ec.clear();
        fs::rename(temporary, destination, ec);
    }
    if (ec) logError("Unable to finalize plugin automation configuration: " + ec.message(),
                     "plugins_automation");
}

} // namespace elizaos
