#include "elizaos/registry.hpp"
#include "elizaos/agentlogger.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <utility>

#ifdef LIBCURL_FOUND
#include <curl/curl.h>
#endif

namespace elizaos {
namespace {

constexpr std::size_t kMaxRegistryBytes = 16U * 1024U * 1024U;
constexpr std::size_t kMaxEntries = 100000U;
constexpr std::size_t kMaxText = 4096U;
constexpr std::size_t kMaxTags = 128U;
constexpr int kSchemaVersion = 1;
constexpr const char* kSchemaName = "elizaos.registry";

std::shared_ptr<Registry> g_globalRegistry;
std::mutex g_globalRegistryMutex;
std::atomic<unsigned long long> g_tempCounter{0};

struct HttpResponse {
    std::string data;
    long responseCode = 0;
    bool overflow = false;
};

#ifdef LIBCURL_FOUND
std::size_t writeCallback(void* contents, std::size_t size, std::size_t nmemb,
                          HttpResponse* response) {
    const std::size_t total = size * nmemb;
    if (total > kMaxRegistryBytes - response->data.size()) {
        response->overflow = true;
        return 0;
    }
    response->data.append(static_cast<char*>(contents), total);
    return total;
}
#endif

bool isIdentifier(const std::string& value) {
    if (value.empty() || value.size() > 128U) {
        return false;
    }
    const auto validSegment = [](const std::string& segment) {
        if (segment.empty() || segment.size() > 128U ||
            !std::isalnum(static_cast<unsigned char>(segment.front()))) {
            return false;
        }
        return std::all_of(segment.begin(), segment.end(), [](unsigned char c) {
            return std::isalnum(c) || c == '.' || c == '_' || c == '-';
        });
    };
    if (value.front() != '@') {
        return validSegment(value);
    }
    const auto slash = value.find('/');
    return slash != std::string::npos && slash > 1U &&
           validSegment(value.substr(1U, slash - 1U)) &&
           validSegment(value.substr(slash + 1U));
}

bool hasControl(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](unsigned char c) {
        return c < 0x20U || c == 0x7fU;
    });
}

bool isText(const std::string& value, bool allowEmpty = false) {
    return value.size() <= kMaxText && !hasControl(value) &&
           (allowEmpty || !value.empty());
}

bool isHttpUrlValue(const std::string& url) {
    const bool scheme = url.rfind("https://", 0) == 0 || url.rfind("http://", 0) == 0;
    if (!scheme || url.size() > 2048U || hasControl(url) || url.find(' ') != std::string::npos) {
        return false;
    }
    const auto authorityStart = url.find("//") + 2U;
    const auto authorityEnd = url.find_first_of("/?#", authorityStart);
    const std::string authority = url.substr(authorityStart, authorityEnd - authorityStart);
    if (authority.empty() || authority.find('@') != std::string::npos) {
        return false;
    }
    const auto colon = authority.rfind(':');
    const std::string host = colon == std::string::npos ? authority : authority.substr(0, colon);
    if (host.empty() || host.front() == '.' || host.back() == '.' ||
        !std::all_of(host.begin(), host.end(), [](unsigned char c) {
            return std::isalnum(c) || c == '.' || c == '-';
        })) {
        return false;
    }
    if (colon != std::string::npos) {
        const std::string port = authority.substr(colon + 1U);
        if (port.empty() || !std::all_of(port.begin(), port.end(), ::isdigit)) {
            return false;
        }
        try {
            const int parsedPort = std::stoi(port);
            if (parsedPort < 1 || parsedPort > 65535) {
                return false;
            }
        } catch (...) {
            return false;
        }
    }
    return true;
}

struct SemVer {
    unsigned long long major = 0;
    unsigned long long minor = 0;
    unsigned long long patch = 0;
    std::vector<std::string> prerelease;
};

bool parseNumber(const std::string& text, unsigned long long& result) {
    if (text.empty() || (text.size() > 1U && text.front() == '0') ||
        !std::all_of(text.begin(), text.end(), ::isdigit)) {
        return false;
    }
    try {
        std::size_t consumed = 0;
        result = std::stoull(text, &consumed);
        return consumed == text.size();
    } catch (...) {
        return false;
    }
}

std::vector<std::string> split(const std::string& value, char delimiter) {
    std::vector<std::string> result;
    std::size_t start = 0;
    while (true) {
        const auto end = value.find(delimiter, start);
        result.push_back(value.substr(start, end - start));
        if (end == std::string::npos) {
            return result;
        }
        start = end + 1U;
    }
}

bool parseSemVer(const std::string& value, SemVer& parsed) {
    if (value.empty() || value.size() > 128U || hasControl(value)) {
        return false;
    }
    const auto plus = value.find('+');
    if (plus != std::string::npos && value.find('+', plus + 1U) != std::string::npos) {
        return false;
    }
    const std::string withoutBuild = value.substr(0, plus);
    if (plus != std::string::npos) {
        const auto buildParts = split(value.substr(plus + 1U), '.');
        if (buildParts.empty() || std::any_of(buildParts.begin(), buildParts.end(), [](const auto& part) {
                return part.empty() || !std::all_of(part.begin(), part.end(), [](unsigned char c) {
                    return std::isalnum(c) || c == '-';
                });
            })) {
            return false;
        }
    }
    const auto dash = withoutBuild.find('-');
    const std::string core = withoutBuild.substr(0, dash);
    const auto coreParts = split(core, '.');
    if (coreParts.size() != 3U || !parseNumber(coreParts[0], parsed.major) ||
        !parseNumber(coreParts[1], parsed.minor) || !parseNumber(coreParts[2], parsed.patch)) {
        return false;
    }
    parsed.prerelease.clear();
    if (dash != std::string::npos) {
        parsed.prerelease = split(withoutBuild.substr(dash + 1U), '.');
        if (parsed.prerelease.empty()) {
            return false;
        }
        for (const auto& part : parsed.prerelease) {
            if (part.empty() || !std::all_of(part.begin(), part.end(), [](unsigned char c) {
                    return std::isalnum(c) || c == '-';
                })) {
                return false;
            }
            const bool numeric = std::all_of(part.begin(), part.end(), ::isdigit);
            if (numeric && part.size() > 1U && part.front() == '0') {
                return false;
            }
        }
    }
    return true;
}

int compareParsed(const SemVer& lhs, const SemVer& rhs) {
    if (lhs.major != rhs.major) return lhs.major < rhs.major ? -1 : 1;
    if (lhs.minor != rhs.minor) return lhs.minor < rhs.minor ? -1 : 1;
    if (lhs.patch != rhs.patch) return lhs.patch < rhs.patch ? -1 : 1;
    if (lhs.prerelease.empty() != rhs.prerelease.empty()) {
        return lhs.prerelease.empty() ? 1 : -1;
    }
    for (std::size_t i = 0; i < std::min(lhs.prerelease.size(), rhs.prerelease.size()); ++i) {
        const auto& a = lhs.prerelease[i];
        const auto& b = rhs.prerelease[i];
        if (a == b) continue;
        const bool aNumeric = std::all_of(a.begin(), a.end(), ::isdigit);
        const bool bNumeric = std::all_of(b.begin(), b.end(), ::isdigit);
        if (aNumeric && bNumeric) {
            unsigned long long av = 0;
            unsigned long long bv = 0;
            if (!parseNumber(a, av) || !parseNumber(b, bv)) {
                return a < b ? -1 : 1;
            }
            return av < bv ? -1 : 1;
        }
        if (aNumeric != bNumeric) return aNumeric ? -1 : 1;
        return a < b ? -1 : 1;
    }
    if (lhs.prerelease.size() == rhs.prerelease.size()) return 0;
    return lhs.prerelease.size() < rhs.prerelease.size() ? -1 : 1;
}

bool jsonHasOnly(const nlohmann::json& value, const std::set<std::string>& keys) {
    if (!value.is_object() || value.size() != keys.size()) return false;
    for (auto it = value.cbegin(); it != value.cend(); ++it) {
        if (keys.count(it.key()) == 0U) return false;
    }
    return true;
}

bool equalEntry(const RegistryEntry& a, const RegistryEntry& b) {
    return a.name == b.name && a.repositoryUrl == b.repositoryUrl &&
           a.description == b.description && a.version == b.version &&
           a.author == b.author && a.tags == b.tags &&
           a.dependencies == b.dependencies && a.metadata == b.metadata;
}

bool validateEntryShape(const RegistryEntry& entry) {
    SemVer parsed;
    if (!isIdentifier(entry.name) || !isHttpUrlValue(entry.repositoryUrl) ||
        !isText(entry.description) || !parseSemVer(entry.version, parsed) ||
        !isText(entry.author) || entry.tags.size() > kMaxTags ||
        entry.metadata.size() > 128U || entry.dependencies.size() > 128U) {
        return false;
    }
    std::set<std::string> tags;
    for (const auto& tag : entry.tags) {
        if (!isIdentifier(tag) || !tags.insert(tag).second) return false;
    }
    for (const auto& dependency : entry.dependencies) {
        SemVer dependencyVersion;
        if (!isIdentifier(dependency.first) || !parseSemVer(dependency.second, dependencyVersion)) {
            return false;
        }
    }
    for (const auto& item : entry.metadata) {
        if (!isIdentifier(item.first) || !isText(item.second, true)) return false;
    }
    return true;
}

bool validateGraph(const std::unordered_map<std::string, RegistryEntry>& entries) {
    for (const auto& pair : entries) {
        if (!validateEntryShape(pair.second) || pair.first != pair.second.name) return false;
        for (const auto& dependency : pair.second.dependencies) {
            const auto found = entries.find(dependency.first);
            if (found == entries.end() ||
                Registry::compareSemanticVersions(found->second.version, dependency.second) != 0) {
                return false;
            }
        }
    }
    enum class Mark { unvisited, visiting, visited };
    std::unordered_map<std::string, Mark> marks;
    for (const auto& pair : entries) marks.emplace(pair.first, Mark::unvisited);
    std::function<bool(const std::string&)> visit = [&](const std::string& name) {
        auto& mark = marks.at(name);
        if (mark == Mark::visiting) return false;
        if (mark == Mark::visited) return true;
        mark = Mark::visiting;
        for (const auto& dependency : entries.at(name).dependencies) {
            if (!visit(dependency.first)) return false;
        }
        mark = Mark::visited;
        return true;
    };
    for (const auto& pair : entries) {
        if (!visit(pair.first)) return false;
    }
    return true;
}

bool parseEntryJson(const nlohmann::json& value, RegistryEntry& entry) {
    static const std::set<std::string> keys = {
        "author", "dependencies", "description", "metadata", "name",
        "repositoryUrl", "tags", "version"};
    if (!jsonHasOnly(value, keys)) return false;
    try {
        entry.name = value.at("name").get<std::string>();
        entry.repositoryUrl = value.at("repositoryUrl").get<std::string>();
        entry.description = value.at("description").get<std::string>();
        entry.version = value.at("version").get<std::string>();
        entry.author = value.at("author").get<std::string>();
        entry.tags = value.at("tags").get<std::vector<std::string>>();
        entry.dependencies = value.at("dependencies").get<std::map<std::string, std::string>>();
        entry.metadata = value.at("metadata").get<std::map<std::string, std::string>>();
        return validateEntryShape(entry);
    } catch (...) {
        return false;
    }
}

nlohmann::json entryToJson(const RegistryEntry& entry) {
    return {{"name", entry.name}, {"repositoryUrl", entry.repositoryUrl},
            {"description", entry.description}, {"version", entry.version},
            {"author", entry.author}, {"tags", entry.tags},
            {"dependencies", entry.dependencies}, {"metadata", entry.metadata}};
}

bool pathHasSymlink(const std::filesystem::path& path) {
    std::filesystem::path current;
    for (const auto& part : path.lexically_normal()) {
        current /= part;
        std::error_code ec;
        const auto status = std::filesystem::symlink_status(current, ec);
        if (!ec && std::filesystem::is_symlink(status)) return true;
    }
    return false;
}

bool writeAtomically(const std::filesystem::path& target, const std::string& data) {
    std::error_code ec;
    const auto parent = target.parent_path();
    if (parent.empty() || pathHasSymlink(parent) ||
        (std::filesystem::exists(target, ec) && std::filesystem::is_symlink(
             std::filesystem::symlink_status(target, ec)))) {
        return false;
    }
    std::filesystem::create_directories(parent, ec);
    if (ec || pathHasSymlink(parent)) return false;
    const auto temp = parent / (target.filename().string() + ".tmp." +
                                std::to_string(g_tempCounter.fetch_add(1)));
    {
        std::ofstream file(temp, std::ios::binary | std::ios::trunc);
        if (!file) return false;
        file.write(data.data(), static_cast<std::streamsize>(data.size()));
        file.flush();
        if (!file) {
            file.close();
            std::filesystem::remove(temp, ec);
            return false;
        }
    }
    if (std::rename(temp.string().c_str(), target.string().c_str()) != 0) {
        std::filesystem::remove(temp, ec);
        return false;
    }
    return true;
}

bool readBounded(const std::filesystem::path& path, std::string& content) {
    std::error_code ec;
    const auto status = std::filesystem::symlink_status(path, ec);
    if (ec || std::filesystem::is_symlink(status) || !std::filesystem::is_regular_file(status)) {
        return false;
    }
    const auto size = std::filesystem::file_size(path, ec);
    if (ec || size > kMaxRegistryBytes) return false;
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return file.good() || file.eof();
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool validateConfig(const RegistryConfig& config) {
    return !config.cacheDirectory.empty() && config.cacheTtlSeconds > 0 &&
           config.cacheTtlSeconds <= 30 * 24 * 60 * 60 &&
           (!config.enableRemoteRegistry || isHttpUrlValue(config.registryUrl)) &&
           (config.registryUrl.empty() || isHttpUrlValue(config.registryUrl));
}

} // namespace

Registry::Registry() : config_(), lastRefreshTime_() {}

Registry::Registry(const RegistryConfig& config) : config_(config), lastRefreshTime_() {
    if (!validateConfig(config)) throw std::invalid_argument("invalid registry configuration");
}

Registry::~Registry() = default;

std::future<bool> Registry::refreshRegistry() {
    std::promise<bool> promise;
    bool result = false;
    const RegistryConfig config = getConfigSnapshot();
    if (config.enableRemoteRegistry && isHttpUrl(config.registryUrl)) {
        const std::string registryData = downloadRegistryData(config.registryUrl);
        if (!registryData.empty() && parseRegistryJson(registryData)) {
            result = saveToCache();
            if (result) updateLastRefreshTime();
        }
    }
    if (!result) result = loadFromCache();
    promise.set_value(result);
    return promise.get_future();
}

bool Registry::loadLocalRegistry(const std::string& registryFilePath) {
    const std::filesystem::path path = registryFilePath.empty()
        ? std::filesystem::path("registry/index.json")
        : std::filesystem::path(registryFilePath);
    std::string content;
    if (!readBounded(path, content) || !parseRegistryJson(content)) return false;
    updateLastRefreshTime();
    return true;
}

bool Registry::registerEntry(const RegistryEntry& entry) {
    if (!validateEntryShape(entry)) return false;
    std::lock_guard<std::mutex> lock(registryMutex_);
    const auto existing = entries_.find(entry.name);
    if (existing != entries_.end()) return equalEntry(existing->second, entry);
    auto staged = entries_;
    staged.emplace(entry.name, entry);
    if (!validateGraph(staged)) return false;
    entries_.swap(staged);
    return true;
}

bool Registry::unregisterEntry(const std::string& name) {
    if (!isIdentifier(name)) return false;
    std::lock_guard<std::mutex> lock(registryMutex_);
    if (entries_.count(name) == 0U) return false;
    for (const auto& pair : entries_) {
        if (pair.second.dependencies.count(name) != 0U) return false;
    }
    entries_.erase(name);
    return true;
}

std::vector<RegistryEntry> Registry::getAllPlugins() const {
    std::vector<RegistryEntry> plugins;
    {
        std::lock_guard<std::mutex> lock(registryMutex_);
        plugins.reserve(entries_.size());
        for (const auto& pair : entries_) plugins.push_back(pair.second);
    }
    std::sort(plugins.begin(), plugins.end(), [](const auto& a, const auto& b) {
        return a.name < b.name;
    });
    return plugins;
}

std::vector<RegistryEntry> Registry::searchPlugins(const std::string& query) const {
    const std::string needle = lower(query);
    auto plugins = getAllPlugins();
    plugins.erase(std::remove_if(plugins.begin(), plugins.end(), [&](const RegistryEntry& entry) {
        if (needle.empty()) return false;
        if (lower(entry.name).find(needle) != std::string::npos ||
            lower(entry.description).find(needle) != std::string::npos ||
            lower(entry.author).find(needle) != std::string::npos) return false;
        return std::none_of(entry.tags.begin(), entry.tags.end(), [&](const std::string& tag) {
            return lower(tag).find(needle) != std::string::npos;
        });
    }), plugins.end());
    return plugins;
}

std::vector<RegistryEntry> Registry::getPluginsSortedByVersion() const {
    auto plugins = getAllPlugins();
    std::sort(plugins.begin(), plugins.end(), [](const auto& a, const auto& b) {
        const int comparison = Registry::compareSemanticVersions(a.version, b.version);
        return comparison == 0 ? a.name < b.name : comparison > 0;
    });
    return plugins;
}

std::optional<RegistryEntry> Registry::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    const auto found = entries_.find(name);
    return found == entries_.end() ? std::nullopt : std::optional<RegistryEntry>(found->second);
}

bool Registry::isRegistryCached() const {
    const RegistryConfig config = getConfigSnapshot();
    const std::filesystem::path path = getCacheFilePath(config);
    std::error_code ec;
    const auto status = std::filesystem::symlink_status(path, ec);
    if (ec || pathHasSymlink(path) || !std::filesystem::is_regular_file(status) ||
        std::filesystem::is_symlink(status)) {
        return false;
    }
    const auto modified = std::filesystem::last_write_time(path, ec);
    if (ec) return false;
    const auto age = std::filesystem::file_time_type::clock::now() - modified;
    return age >= decltype(age)::zero() && age <= std::chrono::seconds(config.cacheTtlSeconds);
}

bool Registry::saveCacheSnapshot() const { return saveToCache(); }

std::string Registry::getLastRefreshTime() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    return lastRefreshTime_;
}

std::size_t Registry::getPluginCount() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    return entries_.size();
}

void Registry::setConfig(const RegistryConfig& config) {
    if (!validateConfig(config)) throw std::invalid_argument("invalid registry configuration");
    std::lock_guard<std::mutex> lock(registryMutex_);
    config_ = config;
}

bool Registry::trySetConfig(const RegistryConfig& config) noexcept {
    try {
        setConfig(config);
        return true;
    } catch (...) {
        return false;
    }
}

RegistryConfig Registry::getConfigSnapshot() const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    return config_;
}

const RegistryConfig& Registry::getConfig() const {
    thread_local RegistryConfig snapshot;
    snapshot = getConfigSnapshot();
    return snapshot;
}

int Registry::compareSemanticVersions(const std::string& lhs, const std::string& rhs) {
    SemVer left;
    SemVer right;
    if (!parseSemVer(lhs, left) || !parseSemVer(rhs, right)) {
        throw std::invalid_argument("invalid semantic version");
    }
    return compareParsed(left, right);
}

bool Registry::parseRegistryJson(const std::string& jsonContent) {
    if (jsonContent.empty() || jsonContent.size() > kMaxRegistryBytes) return false;
    try {
        const auto json = nlohmann::json::parse(jsonContent);
        static const std::set<std::string> rootKeys = {"entries", "schema", "version"};
        if (!jsonHasOnly(json, rootKeys) || json.at("schema") != kSchemaName ||
            json.at("version") != kSchemaVersion || !json.at("entries").is_array() ||
            json.at("entries").size() > kMaxEntries) {
            return false;
        }
        std::unordered_map<std::string, RegistryEntry> staged;
        staged.reserve(json.at("entries").size());
        for (const auto& value : json.at("entries")) {
            RegistryEntry entry;
            if (!parseEntryJson(value, entry) || !staged.emplace(entry.name, entry).second) {
                return false;
            }
        }
        if (!validateGraph(staged)) return false;
        std::lock_guard<std::mutex> lock(registryMutex_);
        entries_.swap(staged);
        return true;
    } catch (...) {
        return false;
    }
}

bool Registry::loadFromCache() {
    if (!isRegistryCached()) return false;
    const RegistryConfig config = getConfigSnapshot();
    std::string content;
    if (!readBounded(getCacheFilePath(config), content) || !parseRegistryJson(content)) return false;
    updateLastRefreshTime();
    return true;
}

bool Registry::saveToCache() const {
    const RegistryConfig config = getConfigSnapshot();
    const auto entries = getAllPlugins();
    nlohmann::json json = {{"schema", kSchemaName}, {"version", kSchemaVersion},
                           {"entries", nlohmann::json::array()}};
    for (const auto& entry : entries) json["entries"].push_back(entryToJson(entry));
    return writeAtomically(getCacheFilePath(config), json.dump(2));
}

std::string Registry::getCacheFilePath(const RegistryConfig& config) const {
    return (std::filesystem::path(expandPath(config.cacheDirectory)) /
            "registry_index.json").lexically_normal().string();
}

bool Registry::isHttpUrl(const std::string& url) const { return isHttpUrlValue(url); }

std::string Registry::downloadRegistryData(const std::string& url) const {
#ifdef LIBCURL_FOUND
    if (!isHttpUrl(url)) return {};
    CURL* curl = curl_easy_init();
    if (curl == nullptr) return {};
    HttpResponse response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
#if LIBCURL_VERSION_NUM >= 0x075500
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "http,https");
#else
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
    const CURLcode result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.responseCode);
    curl_easy_cleanup(curl);
    return result == CURLE_OK && response.responseCode == 200 && !response.overflow
        ? response.data : std::string{};
#else
    (void)url;
    return {};
#endif
}

std::string Registry::expandPath(const std::string& path) const {
    if (path.empty() || path.front() != '~') return path;
    if (path.size() > 1U && path[1] != '/') return {};
    const char* home = std::getenv("HOME");
    return home == nullptr ? std::string{} : std::string(home) + path.substr(1U);
}

void Registry::updateLastRefreshTime() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &time);
#else
    gmtime_r(&time, &utc);
#endif
    std::ostringstream stream;
    stream << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    const std::string timestamp = stream.str();
    std::lock_guard<std::mutex> lock(registryMutex_);
    lastRefreshTime_ = timestamp;
}

std::shared_ptr<Registry> getGlobalRegistryShared() {
    std::lock_guard<std::mutex> lock(g_globalRegistryMutex);
    if (!g_globalRegistry) g_globalRegistry = std::make_shared<Registry>();
    return g_globalRegistry;
}

Registry& getGlobalRegistry() {
    thread_local std::shared_ptr<Registry> pinned;
    pinned = getGlobalRegistryShared();
    return *pinned;
}

void setGlobalRegistry(std::shared_ptr<Registry> registry) {
    if (!registry) registry = std::make_shared<Registry>();
    std::lock_guard<std::mutex> lock(g_globalRegistryMutex);
    g_globalRegistry = std::move(registry);
}

void setGlobalRegistry(std::unique_ptr<Registry> registry) {
    setGlobalRegistry(std::shared_ptr<Registry>(std::move(registry)));
}

} // namespace elizaos
