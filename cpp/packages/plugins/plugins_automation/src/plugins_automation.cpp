#include "elizaos/plugins_automation.hpp"
#include "elizaos/agentlogger.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <future>
#include <set>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <thread>

#include <nlohmann/json.hpp>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace elizaos {
namespace {

namespace fs = std::filesystem;
using json = nlohmann::json;

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string shellQuote(const std::string& value) {
#ifdef _WIN32
    std::string quoted = "\"";
    for (char c : value) {
        if (c == '"') {
            quoted += "\\\"";
        } else {
            quoted += c;
        }
    }
    quoted += '"';
    return quoted;
#else
    std::string quoted = "'";
    for (char c : value) {
        if (c == '\'') {
            quoted += "'\\''";
        } else {
            quoted += c;
        }
    }
    quoted += '\'';
    return quoted;
#endif
}

void replaceAll(std::string& value, const std::string& token, const std::string& replacement) {
    if (token.empty()) {
        return;
    }
    std::size_t position = 0;
    while ((position = value.find(token, position)) != std::string::npos) {
        value.replace(position, token.size(), replacement);
        position += replacement.size();
    }
}

std::string identifierFor(const std::string& value) {
    std::string identifier;
    identifier.reserve(value.size() + 1);
    for (char c : value) {
        const auto uc = static_cast<unsigned char>(c);
        identifier += std::isalnum(uc) ? static_cast<char>(c) : '_';
    }
    if (identifier.empty()) {
        return "Plugin";
    }
    if (std::isdigit(static_cast<unsigned char>(identifier.front()))) {
        identifier.insert(identifier.begin(), '_');
    }
    return identifier;
}

bool writeTextFile(const fs::path& path, const std::string& content, std::string* error = nullptr) {
    std::error_code ec;
    if (path.has_parent_path()) {
        fs::create_directories(path.parent_path(), ec);
        if (ec) {
            if (error) *error = "failed to create directory: " + ec.message();
            return false;
        }
    }
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    if (!stream) {
        if (error) *error = "failed to open file for writing: " + path.string();
        return false;
    }
    stream << content;
    if (!stream.good()) {
        if (error) *error = "failed while writing file: " + path.string();
        return false;
    }
    return true;
}

bool runCommandCaptured(const std::string& command, std::string& output, std::string& error) {
    output.clear();
    error.clear();
    const std::string fullCommand = command + " 2>&1";
#ifdef _WIN32
    FILE* pipe = _popen(fullCommand.c_str(), "r");
#else
    FILE* pipe = popen(fullCommand.c_str(), "r");
#endif
    if (!pipe) {
        error = "failed to launch command";
        return false;
    }

    char buffer[4096];
    while (std::fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output.append(buffer);
    }

#ifdef _WIN32
    const int status = _pclose(pipe);
    const int exitCode = status;
#else
    const int status = pclose(pipe);
    const int exitCode = (status >= 0 && WIFEXITED(status)) ? WEXITSTATUS(status) : status;
#endif
    if (exitCode != 0) {
        error = "command exited with status " + std::to_string(exitCode);
        if (!output.empty()) {
            error += ": " + trim(output);
        }
        return false;
    }
    return true;
}

std::string commandForPath(std::string command, const std::string& path) {
    if (command.find("{plugin}") != std::string::npos) {
        replaceAll(command, "{plugin}", shellQuote(path));
        return command;
    }
    if (!path.empty() && fs::is_directory(fs::path(path))) {
        return "cd " + shellQuote(fs::absolute(path).string()) + " && " + command;
    }
    return command;
}

}  // namespace

std::shared_ptr<PluginsAutomation> globalPluginAutomation = std::make_shared<PluginsAutomation>();

// -----------------------------------------------------------------------------
// PluginRegistry
// -----------------------------------------------------------------------------

PluginRegistry::PluginRegistry() = default;

PluginRegistry::~PluginRegistry() {
    std::vector<std::shared_ptr<Plugin>> plugins;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        for (const auto& [_, plugin] : plugins_) {
            if (plugin) plugins.push_back(plugin);
        }
        plugins_.clear();
        metadata_.clear();
    }
    for (const auto& plugin : plugins) {
        if (plugin->getStatus() == PluginStatus::ACTIVE) {
            plugin->deactivate();
        }
        plugin->shutdown();
    }
}

bool PluginRegistry::registerPlugin(std::shared_ptr<Plugin> plugin, const PluginMetadata& metadata) {
    if (!plugin || metadata.name.empty() || metadata.version.empty()) {
        logError("Invalid plugin or metadata", "plugins_automation");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        if (plugins_.count(metadata.name) != 0) {
            logWarning("Plugin " + metadata.name + " already registered", "plugins_automation");
            return false;
        }
        if (!checkDependencies(metadata)) {
            logError("Dependencies not satisfied for plugin " + metadata.name, "plugins_automation");
            return false;
        }
    }

    if (!plugin->initialize(metadata)) {
        logError("Plugin initialization failed: " + metadata.name, "plugins_automation");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        if (plugins_.count(metadata.name) != 0) {
            plugin->shutdown();
            return false;
        }
        plugins_[metadata.name] = std::move(plugin);
        metadata_[metadata.name] = metadata;
    }

    logInfo("Registered plugin: " + metadata.name + " v" + metadata.version,
            "plugins_automation");
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& name) {
    std::shared_ptr<Plugin> plugin;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            return false;
        }
        for (const auto& [registeredName, metadata] : metadata_) {
            if (registeredName == name) continue;
            if (std::find(metadata.dependencies.begin(), metadata.dependencies.end(), name) !=
                metadata.dependencies.end()) {
                logWarning("Cannot unregister " + name + "; required by " + registeredName,
                           "plugins_automation");
                return false;
            }
        }
        plugin = it->second;
        plugins_.erase(it);
        metadata_.erase(name);
    }

    bool ok = true;
    if (plugin) {
        if (plugin->getStatus() == PluginStatus::ACTIVE) {
            ok = plugin->deactivate() && ok;
        }
        ok = plugin->shutdown() && ok;
    }
    logInfo("Unregistered plugin: " + name, "plugins_automation");
    return ok;
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
    for (const auto& [name, _] : plugins_) names.push_back(name);
    std::sort(names.begin(), names.end());
    return names;
}

PluginStatus PluginRegistry::getPluginStatus(const std::string& name) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = plugins_.find(name);
    return it != plugins_.end() && it->second ? it->second->getStatus() : PluginStatus::UNKNOWN;
}

std::vector<std::string> PluginRegistry::getActivePlugins() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    std::vector<std::string> active;
    for (const auto& [name, plugin] : plugins_) {
        if (plugin && plugin->getStatus() == PluginStatus::ACTIVE) active.push_back(name);
    }
    std::sort(active.begin(), active.end());
    return active;
}

std::vector<std::string> PluginRegistry::getFailedPlugins() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    std::vector<std::string> failed;
    for (const auto& [name, plugin] : plugins_) {
        if (plugin && plugin->getStatus() == PluginStatus::FAILED) failed.push_back(name);
    }
    std::sort(failed.begin(), failed.end());
    return failed;
}

bool PluginRegistry::resolveDependencies(const std::string& pluginName) {
    std::vector<std::shared_ptr<Plugin>> orderedPlugins;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        if (metadata_.count(pluginName) == 0) return false;

        std::set<std::string> visiting;
        std::set<std::string> visited;
        std::function<bool(const std::string&)> visit = [&](const std::string& name) {
            if (visited.count(name)) return true;
            if (!visiting.insert(name).second) return false;
            auto metadataIt = metadata_.find(name);
            auto pluginIt = plugins_.find(name);
            if (metadataIt == metadata_.end() || pluginIt == plugins_.end() || !pluginIt->second) {
                return false;
            }
            for (const auto& dependency : metadataIt->second.dependencies) {
                if (!visit(dependency)) return false;
            }
            visiting.erase(name);
            visited.insert(name);
            orderedPlugins.push_back(pluginIt->second);
            return true;
        };
        if (!visit(pluginName)) return false;
    }

    for (const auto& plugin : orderedPlugins) {
        const auto status = plugin->getStatus();
        if (status == PluginStatus::ACTIVE) continue;
        if (status == PluginStatus::FAILED || !plugin->activate()) return false;
    }
    return true;
}

std::vector<std::string> PluginRegistry::getDependencyChain(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    std::vector<std::string> chain;
    std::set<std::string> visiting;
    std::set<std::string> visited;
    std::function<void(const std::string&)> visit = [&](const std::string& name) {
        if (visited.count(name) || !visiting.insert(name).second) return;
        auto it = metadata_.find(name);
        if (it != metadata_.end()) {
            for (const auto& dependency : it->second.dependencies) {
                visit(dependency);
                if (visited.insert(dependency).second) chain.push_back(dependency);
            }
        }
        visiting.erase(name);
    };
    visit(pluginName);
    return chain;
}

bool PluginRegistry::checkDependencies(const PluginMetadata& metadata) const {
    for (const auto& dependency : metadata.dependencies) {
        auto it = plugins_.find(dependency);
        if (it == plugins_.end() || !it->second) return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// CIPipeline
// -----------------------------------------------------------------------------

CIPipeline::CIPipeline()
    : buildCommand_("cmake --build build"),
      testCommand_("ctest --test-dir build --output-on-failure"),
      deployCommand_("cmake --install build") {}
CIPipeline::~CIPipeline() = default;

std::future<bool> CIPipeline::buildPlugin(const std::string& pluginPath) {
    return std::async(std::launch::async, [this, pluginPath]() {
        std::string command;
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            command = buildCommand_;
            statuses_[pluginPath] = {pluginPath, "build", true, false, {}, {}};
        }
        if (pluginPath.empty() || !fs::exists(fs::path(pluginPath))) {
            std::lock_guard<std::mutex> lock(statusMutex_);
            auto& status = statuses_[pluginPath];
            status.inProgress = false;
            status.error = "plugin path does not exist";
            return false;
        }
        command = commandForPath(command, pluginPath);
        std::string output;
        std::string error;
        const bool ok = executeCommand(command, output, error);
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            auto& status = statuses_[pluginPath];
            status.inProgress = false;
            status.success = ok;
            status.output = output;
            status.error = error;
        }
        return ok;
    });
}

std::future<bool> CIPipeline::testPlugin(const std::string& pluginName) {
    return std::async(std::launch::async, [this, pluginName]() {
        std::string command;
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            command = testCommand_;
            statuses_[pluginName] = {pluginName, "test", true, false, {}, {}};
        }
        if (pluginName.empty()) {
            std::lock_guard<std::mutex> lock(statusMutex_);
            auto& status = statuses_[pluginName];
            status.inProgress = false;
            status.error = "plugin name/path is empty";
            return false;
        }
        command = commandForPath(command, pluginName);
        std::string output;
        std::string error;
        const bool ok = executeCommand(command, output, error);
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            auto& status = statuses_[pluginName];
            status.inProgress = false;
            status.success = ok;
            status.output = output;
            status.error = error;
        }
        return ok;
    });
}

std::future<bool> CIPipeline::deployPlugin(const std::string& pluginName,
                                           const std::string& target) {
    return std::async(std::launch::async, [this, pluginName, target]() {
        std::string command;
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            command = deployCommand_;
            statuses_[pluginName] = {pluginName, "deploy", true, false, {}, {}};
        }
        if (pluginName.empty() || target.empty()) {
            std::lock_guard<std::mutex> lock(statusMutex_);
            auto& status = statuses_[pluginName];
            status.inProgress = false;
            status.error = "plugin name/path and deployment target are required";
            return false;
        }
        replaceAll(command, "{plugin}", shellQuote(pluginName));
        replaceAll(command, "{target}", shellQuote(target));
        if (command.find(shellQuote(pluginName)) == std::string::npos && fs::is_directory(pluginName)) {
            command = "cd " + shellQuote(fs::absolute(pluginName).string()) + " && " + command;
        }
        std::string output;
        std::string error;
        const bool ok = executeCommand(command, output, error);
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            auto& status = statuses_[pluginName];
            status.inProgress = false;
            status.success = ok;
            status.output = output;
            status.error = error;
        }
        return ok;
    });
}

void CIPipeline::setBuildCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    buildCommand_ = trim(command);
}

void CIPipeline::setTestCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    testCommand_ = trim(command);
}

void CIPipeline::setDeployCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    deployCommand_ = trim(command);
}

CIPipeline::PipelineStatus CIPipeline::getStatus(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    auto it = statuses_.find(pluginName);
    if (it != statuses_.end()) return it->second;
    PipelineStatus status{};
    status.pluginName = pluginName;
    status.error = "no pipeline status recorded";
    return status;
}

std::vector<CIPipeline::PipelineStatus> CIPipeline::getAllStatuses() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    std::vector<PipelineStatus> statuses;
    statuses.reserve(statuses_.size());
    for (const auto& [_, status] : statuses_) statuses.push_back(status);
    std::sort(statuses.begin(), statuses.end(), [](const auto& left, const auto& right) {
        return left.pluginName < right.pluginName;
    });
    return statuses;
}

bool CIPipeline::executeCommand(const std::string& command, std::string& output,
                                std::string& error) {
    if (trim(command).empty()) {
        output.clear();
        error = "pipeline command is empty";
        return false;
    }
    return runCommandCaptured(command, output, error);
}

// -----------------------------------------------------------------------------
// PluginTester
// -----------------------------------------------------------------------------

PluginTester::PluginTester() : timeoutSeconds_(30), verbose_(false) {}
PluginTester::~PluginTester() = default;

void PluginTester::addTestCase(const std::string& testName, std::function<bool()> testFunc) {
    if (testName.empty() || !testFunc) return;
    std::lock_guard<std::mutex> lock(testMutex_);
    testCases_[testName] = std::move(testFunc);
}

void PluginTester::removeTestCase(const std::string& testName) {
    std::lock_guard<std::mutex> lock(testMutex_);
    testCases_.erase(testName);
}

std::vector<PluginTester::TestResult> PluginTester::runTests(const std::string& pluginName) {
    std::vector<std::pair<std::string, std::function<bool()>>> tests;
    {
        std::lock_guard<std::mutex> lock(testMutex_);
        const std::string prefix = pluginName + "::";
        for (const auto& [name, test] : testCases_) {
            if (name == pluginName || name.rfind(prefix, 0) == 0) tests.emplace_back(name, test);
        }
    }
    std::sort(tests.begin(), tests.end(), [](const auto& left, const auto& right) {
        return left.first < right.first;
    });
    std::vector<TestResult> results;
    results.reserve(tests.size());
    for (const auto& [name, test] : tests) results.push_back(executeTest(name, test));
    return results;
}

std::vector<PluginTester::TestResult> PluginTester::runAllTests() {
    std::vector<std::pair<std::string, std::function<bool()>>> tests;
    {
        std::lock_guard<std::mutex> lock(testMutex_);
        tests.assign(testCases_.begin(), testCases_.end());
    }
    std::sort(tests.begin(), tests.end(), [](const auto& left, const auto& right) {
        return left.first < right.first;
    });
    std::vector<TestResult> results;
    results.reserve(tests.size());
    for (const auto& [name, test] : tests) results.push_back(executeTest(name, test));
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

PluginTester::TestResult PluginTester::executeTest(const std::string& testName,
                                                   std::function<bool()> testFunc) {
    const auto started = std::chrono::steady_clock::now();
    int timeoutSeconds = 0;
    bool verbose = false;
    {
        std::lock_guard<std::mutex> lock(testMutex_);
        timeoutSeconds = timeoutSeconds_;
        verbose = verbose_;
    }

    if (!testFunc) return TestResult(testName, false, "test function is empty", 0.0);

    std::packaged_task<bool()> task(std::move(testFunc));
    auto future = task.get_future();
    std::thread(std::move(task)).detach();

    const auto waitStatus = future.wait_for(std::chrono::seconds(timeoutSeconds));
    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
    if (waitStatus != std::future_status::ready) {
        return TestResult(testName, false, "test timed out after " +
                          std::to_string(timeoutSeconds) + " second(s)", elapsed);
    }

    try {
        const bool passed = future.get();
        const std::string message = passed ? "passed" : "test returned false";
        if (verbose) {
            logInfo("Plugin test " + testName + ": " + message, "plugins_automation");
        }
        return TestResult(testName, passed, message, elapsed);
    } catch (const std::exception& ex) {
        return TestResult(testName, false, std::string("exception: ") + ex.what(), elapsed);
    } catch (...) {
        return TestResult(testName, false, "unknown exception", elapsed);
    }
}

// -----------------------------------------------------------------------------
// WorkflowAutomation
// -----------------------------------------------------------------------------

WorkflowAutomation::WorkflowAutomation() : templateDirectory_("templates") {}
WorkflowAutomation::~WorkflowAutomation() = default;

bool WorkflowAutomation::createPluginTemplate(const std::string& pluginName,
                                              const std::string& outputPath) {
    return createPluginTemplate(pluginName, "basic_template", outputPath);
}

bool WorkflowAutomation::createPluginTemplate(const std::string& pluginName,
                                              const std::string& templateName,
                                              const std::string& outputPath) {
    if (pluginName.empty() || outputPath.empty()) return false;
    if (fs::path(templateName).filename().string() != templateName) return false;

    const fs::path destination = fs::path(outputPath) / pluginName;
    std::error_code ec;
    if (fs::exists(destination, ec)) return false;

    const fs::path configuredTemplate = fs::path(templateDirectory_) / templateName;
    if (fs::is_directory(configuredTemplate, ec)) {
        if (!copyTemplate(templateName, destination.string())) return false;
    } else {
        fs::create_directories(destination / "src", ec);
        if (ec) return false;
        fs::create_directories(destination / "include", ec);
        if (ec) return false;
        fs::create_directories(destination / "tests", ec);
        if (ec) return false;

        const std::string identifier = identifierFor(pluginName);
        const std::string cmake =
            "cmake_minimum_required(VERSION 3.16)\n"
            "project(" + identifier + " LANGUAGES CXX)\n"
            "add_library(" + identifier + " STATIC src/" + pluginName + ".cpp)\n"
            "target_include_directories(" + identifier + " PUBLIC include)\n";
        const std::string header =
            "#pragma once\n\nnamespace elizaos {\nclass " + identifier +
            " {\npublic:\n    bool initialize();\n};\n}\n";
        const std::string source =
            "#include \"" + pluginName + ".hpp\"\n\nnamespace elizaos {\n"
            "bool " + identifier + "::initialize() { return true; }\n}\n";
        if (!writeTextFile(destination / "CMakeLists.txt", cmake) ||
            !writeTextFile(destination / "include" / (pluginName + ".hpp"), header) ||
            !writeTextFile(destination / "src" / (pluginName + ".cpp"), source)) {
            fs::remove_all(destination, ec);
            return false;
        }
    }

    const std::unordered_map<std::string, std::string> tokens = {
        {"{{PLUGIN_NAME}}", pluginName},
        {"{{PLUGIN_IDENTIFIER}}", identifierFor(pluginName)},
        {"${PLUGIN_NAME}", pluginName},
    };
    for (fs::recursive_directory_iterator it(destination, ec), end; !ec && it != end; it.increment(ec)) {
        if (it->is_regular_file() && !replaceTokens(it->path().string(), tokens)) return false;
    }
    return !ec;
}

bool WorkflowAutomation::generatePluginDocs(const std::string& pluginPath) {
    const fs::path path(pluginPath);
    if (!fs::is_directory(path)) return false;
    const std::string name = path.filename().string();
    const std::string documentation =
        "# " + name + "\n\n"
        "Generated ElizaOS C++ plugin package.\n\n"
        "## Build\n\n```sh\ncmake -S . -B build\ncmake --build build\n```\n";
    return writeTextFile(path / "README.md", documentation);
}

bool WorkflowAutomation::validatePluginStructure(const std::string& pluginPath) {
    const fs::path path(pluginPath);
    if (!fs::is_directory(path) || !fs::is_regular_file(path / "CMakeLists.txt")) return false;
    const fs::path sourceDir = path / "src";
    const fs::path includeDir = path / "include";
    if (!fs::is_directory(sourceDir) || !fs::is_directory(includeDir)) return false;

    bool hasSource = false;
    bool hasHeader = false;
    std::error_code ec;
    for (fs::directory_iterator it(sourceDir, ec), end; !ec && it != end; it.increment(ec)) {
        const auto extension = it->path().extension().string();
        hasSource = hasSource || (it->is_regular_file() &&
            (extension == ".cpp" || extension == ".cc" || extension == ".cxx"));
    }
    for (fs::directory_iterator it(includeDir, ec), end; !ec && it != end; it.increment(ec)) {
        const auto extension = it->path().extension().string();
        hasHeader = hasHeader || (it->is_regular_file() &&
            (extension == ".hpp" || extension == ".h" || extension == ".hxx"));
    }
    return !ec && hasSource && hasHeader;
}

bool WorkflowAutomation::packagePlugin(const std::string& pluginPath,
                                       const std::string& outputPath) {
    const fs::path source = fs::absolute(pluginPath);
    if (!validatePluginStructure(source.string()) || outputPath.empty()) return false;

    fs::path archive(outputPath);
    if (archive.extension() != ".tgz" && archive.extension() != ".gz") {
        std::error_code ec;
        fs::create_directories(archive, ec);
        if (ec) return false;
        archive /= source.filename().string() + ".tar.gz";
    } else if (archive.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(archive.parent_path(), ec);
        if (ec) return false;
    }

    std::string output;
    std::string error;
    const std::string command = "tar -czf " + shellQuote(fs::absolute(archive).string()) +
                                " -C " + shellQuote(source.parent_path().string()) +
                                " " + shellQuote(source.filename().string());
    return runCommandCaptured(command, output, error) && fs::is_regular_file(archive);
}

bool WorkflowAutomation::generateInterface(const std::string& interfaceName,
                                           const std::string& outputPath) {
    if (interfaceName.empty() || outputPath.empty()) return false;
    fs::path destination(outputPath);
    if (destination.extension().empty()) destination /= interfaceName + ".hpp";
    const std::string identifier = identifierFor(interfaceName);
    const std::string content =
        "#pragma once\n\nnamespace elizaos {\nclass " + identifier +
        " {\npublic:\n    virtual ~" + identifier + "() = default;\n"
        "    virtual bool initialize() = 0;\n};\n}\n";
    return writeTextFile(destination, content);
}

bool WorkflowAutomation::generateTestSkeleton(const std::string& pluginName,
                                              const std::string& outputPath) {
    if (pluginName.empty() || outputPath.empty()) return false;
    fs::path destination(outputPath);
    if (destination.extension().empty()) destination /= pluginName + "_test.cpp";
    const std::string identifier = identifierFor(pluginName);
    const std::string content =
        "#include <gtest/gtest.h>\n#include \"" + pluginName + ".hpp\"\n\n"
        "TEST(" + identifier + ", Initializes) {\n"
        "    SUCCEED();\n"
        "}\n";
    return writeTextFile(destination, content);
}

void WorkflowAutomation::setTemplateDirectory(const std::string& path) {
    templateDirectory_ = path;
}

std::vector<std::string> WorkflowAutomation::getAvailableTemplates() const {
    std::vector<std::string> templates;
    std::error_code ec;
    if (!fs::is_directory(templateDirectory_, ec)) return templates;
    for (fs::directory_iterator it(templateDirectory_, ec), end; !ec && it != end; it.increment(ec)) {
        if (it->is_directory()) templates.push_back(it->path().filename().string());
    }
    std::sort(templates.begin(), templates.end());
    return templates;
}

bool WorkflowAutomation::copyTemplate(const std::string& templateName,
                                      const std::string& destination) {
    const fs::path source = fs::path(templateDirectory_) / templateName;
    if (!fs::is_directory(source)) return false;
    std::error_code ec;
    fs::copy(source, destination,
             fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
    return !ec;
}

bool WorkflowAutomation::replaceTokens(
    const std::string& filePath,
    const std::unordered_map<std::string, std::string>& tokens) {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) return false;
    std::ostringstream buffer;
    buffer << input.rdbuf();
    if (!input.good() && !input.eof()) return false;
    std::string content = buffer.str();
    for (const auto& [token, replacement] : tokens) replaceAll(content, token, replacement);
    return writeTextFile(filePath, content);
}

// -----------------------------------------------------------------------------
// PluginsAutomation
// -----------------------------------------------------------------------------

PluginsAutomation::PluginsAutomation() = default;
PluginsAutomation::~PluginsAutomation() = default;

bool PluginsAutomation::automatedPluginSetup(const std::string& pluginName,
                                             const std::string& templateName) {
    if (pluginName.empty()) return false;
    std::string outputRoot = "generated_plugins";
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        auto it = config_.find("output_root");
        if (it != config_.end() && !it->second.empty()) outputRoot = it->second;
    }
    logInfo("Starting automated setup for plugin: " + pluginName, "plugins_automation");
    if (!workflow_.createPluginTemplate(pluginName, templateName, outputRoot)) return false;
    const fs::path pluginPath = fs::path(outputRoot) / pluginName;
    return workflow_.generatePluginDocs(pluginPath.string()) &&
           workflow_.validatePluginStructure(pluginPath.string());
}

bool PluginsAutomation::automatedBuildAndTest(const std::string& pluginPath) {
    if (pluginPath.empty() || !fs::is_directory(pluginPath)) return false;
    logInfo("Starting automated build and test for: " + pluginPath, "plugins_automation");
    if (!pipeline_.buildPlugin(pluginPath).get()) return false;
    return pipeline_.testPlugin(pluginPath).get();
}

bool PluginsAutomation::automatedDeployment(const std::string& pluginName,
                                            const std::string& target) {
    logInfo("Starting automated deployment for: " + pluginName + " to " + target,
            "plugins_automation");
    return pipeline_.deployPlugin(pluginName, target).get();
}

void PluginsAutomation::loadConfiguration(const std::string& configPath) {
    std::ifstream input(configPath);
    if (!input) {
        logWarning("Configuration file does not exist: " + configPath, "plugins_automation");
        return;
    }
    try {
        const json document = json::parse(input);
        if (!document.is_object()) throw std::runtime_error("root must be an object");
        std::unordered_map<std::string, std::string> loaded;
        for (auto it = document.begin(); it != document.end(); ++it) {
            if (!it.value().is_string()) {
                throw std::runtime_error("configuration value for '" + it.key() + "' must be a string");
            }
            loaded[it.key()] = it.value().get<std::string>();
        }
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            config_ = loaded;
        }
        if (loaded.count("build_command")) pipeline_.setBuildCommand(loaded.at("build_command"));
        if (loaded.count("test_command")) pipeline_.setTestCommand(loaded.at("test_command"));
        if (loaded.count("deploy_command")) pipeline_.setDeployCommand(loaded.at("deploy_command"));
        if (loaded.count("template_directory")) workflow_.setTemplateDirectory(loaded.at("template_directory"));
        logInfo("Loaded configuration from: " + configPath, "plugins_automation");
    } catch (const std::exception& ex) {
        logError("Failed to load configuration '" + configPath + "': " + ex.what(),
                 "plugins_automation");
    }
}

void PluginsAutomation::saveConfiguration(const std::string& configPath) {
    std::unordered_map<std::string, std::string> snapshot;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        snapshot = config_;
    }
    const json document(snapshot);
    std::string error;
    if (!writeTextFile(configPath, document.dump(2) + "\n", &error)) {
        logError("Failed to save configuration '" + configPath + "': " + error,
                 "plugins_automation");
        return;
    }
    logInfo("Saved configuration to: " + configPath, "plugins_automation");
}

}  // namespace elizaos
