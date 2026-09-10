#include "elizaos/elizas_list.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <system_error>
#include <unordered_set>
#include <utility>

namespace elizaos {
namespace {

constexpr std::size_t kMaxSnapshotBytes = 16U * 1024U * 1024U;
constexpr std::size_t kMaxProjects = 100000U;
constexpr std::size_t kMaxCollections = 100000U;
constexpr std::size_t kMaxText = 4096U;
constexpr std::size_t kMaxTags = 128U;
constexpr int kSchemaVersion = 1;
constexpr const char* kSchemaName = "elizaos.elizas-list";
std::atomic<unsigned long long> g_tempCounter{0};

bool hasControl(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](unsigned char c) {
        return c < 0x20U || c == 0x7fU;
    });
}

bool isText(const std::string& value, bool allowEmpty = false) {
    return value.size() <= kMaxText && !hasControl(value) &&
           (allowEmpty || !value.empty());
}

bool isIdentifier(const std::string& value) {
    return !value.empty() && value.size() <= 128U &&
           std::isalnum(static_cast<unsigned char>(value.front())) &&
           std::all_of(value.begin(), value.end(), [](unsigned char c) {
               return std::isalnum(c) || c == '.' || c == '_' || c == '-';
           });
}

bool isGithub(const std::string& value) {
    if (value.empty() || value.size() > 128U || hasControl(value) ||
        value.front() == '/' || value.back() == '/') return false;
    const auto parts = [&]() {
        std::vector<std::string> result;
        std::size_t start = 0;
        while (true) {
            const auto end = value.find('/', start);
            result.push_back(value.substr(start, end - start));
            if (end == std::string::npos) return result;
            start = end + 1U;
        }
    }();
    if (parts.empty() || parts.size() > 2U) return false;
    return std::all_of(parts.begin(), parts.end(), [](const std::string& part) {
        return !part.empty() && std::isalnum(static_cast<unsigned char>(part.front())) &&
               std::all_of(part.begin(), part.end(), [](unsigned char c) {
                   return std::isalnum(c) || c == '.' || c == '_' || c == '-';
               });
    });
}

bool isUrl(const std::string& url, bool allowEmpty = false) {
    if (url.empty()) return allowEmpty;
    const bool scheme = url.rfind("https://", 0) == 0 || url.rfind("http://", 0) == 0;
    if (!scheme || url.size() > 2048U || hasControl(url) || url.find(' ') != std::string::npos) {
        return false;
    }
    const auto start = url.find("//") + 2U;
    const auto end = url.find_first_of("/?#", start);
    const std::string authority = url.substr(start, end - start);
    if (authority.empty() || authority.find('@') != std::string::npos) return false;
    const auto colon = authority.rfind(':');
    const std::string host = colon == std::string::npos ? authority : authority.substr(0, colon);
    return !host.empty() && host.front() != '.' && host.back() != '.' &&
           std::all_of(host.begin(), host.end(), [](unsigned char c) {
               return std::isalnum(c) || c == '.' || c == '-';
           });
}

bool isDate(const std::string& value, bool allowEmpty = false) {
    if (value.empty()) return allowEmpty;
    if (value.size() != 10U || value[4] != '-' || value[7] != '-') return false;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (i != 4U && i != 7U && !std::isdigit(static_cast<unsigned char>(value[i]))) return false;
    }
    const int year = std::stoi(value.substr(0, 4));
    const int month = std::stoi(value.substr(5, 2));
    const int day = std::stoi(value.substr(8, 2));
    if (year < 1970 || month < 1 || month > 12 || day < 1) return false;
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maximum = days[month - 1];
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) maximum = 29;
    return day <= maximum;
}

bool isAmount(const std::string& value, bool allowEmpty = false) {
    if (value.empty()) return allowEmpty;
    if (value.size() > 64U) return false;
    const auto point = value.find('.');
    if (point != std::string::npos && value.find('.', point + 1U) != std::string::npos) return false;
    const std::string whole = value.substr(0, point);
    const std::string fraction = point == std::string::npos ? std::string{} : value.substr(point + 1U);
    return !whole.empty() && (whole.size() == 1U || whole.front() != '0') &&
           std::all_of(whole.begin(), whole.end(), ::isdigit) &&
           (point == std::string::npos || (!fraction.empty() && fraction.size() <= 18U &&
            std::all_of(fraction.begin(), fraction.end(), ::isdigit)));
}

bool isTransactionHash(const std::string& value, bool allowEmpty = false) {
    if (value.empty()) return allowEmpty;
    return value.size() >= 8U && value.size() <= 128U &&
           std::all_of(value.begin(), value.end(), [](unsigned char c) {
               return std::isalnum(c) || c == '-' || c == '_';
           });
}

bool validateAuthor(const Author& author) {
    return isText(author.name) && isGithub(author.github) &&
           (!author.twitter || (isText(*author.twitter) && author.twitter->size() <= 64U));
}

bool validateDonation(const Donation& donation) {
    const bool allEmpty = donation.transactionHash.empty() && donation.amount.empty() && donation.date.empty();
    if (allEmpty) return true;
    return isTransactionHash(donation.transactionHash) && isAmount(donation.amount) && isDate(donation.date);
}

bool validateProject(const Project& project) {
    if (!isIdentifier(project.id) || !isText(project.name) || !isText(project.description) ||
        !isUrl(project.projectUrl) || !isGithub(project.github) || !isUrl(project.image, true) ||
        !validateAuthor(project.author) || !validateDonation(project.donation) ||
        !isDate(project.addedOn) || project.tags.size() > kMaxTags) return false;
    if (project.metrics && (project.metrics->stars < 0 || project.metrics->forks < 0)) return false;
    std::set<std::string> tags;
    for (const auto& tag : project.tags) {
        if (!isIdentifier(tag) || !tags.insert(tag).second) return false;
    }
    return true;
}

bool validateCollectionShape(const Collection& collection) {
    if (!isIdentifier(collection.id) || !isText(collection.name) ||
        !isText(collection.description) || !isText(collection.curator.name) ||
        !isGithub(collection.curator.github) || collection.projects.size() > kMaxProjects) return false;
    std::set<std::string> projects;
    for (const auto& id : collection.projects) {
        if (!isIdentifier(id) || !projects.insert(id).second) return false;
    }
    return true;
}

bool validateState(const std::vector<Project>& projects,
                   const std::vector<Collection>& collections) {
    if (projects.size() > kMaxProjects || collections.size() > kMaxCollections) return false;
    std::unordered_set<std::string> projectIds;
    for (const auto& project : projects) {
        if (!validateProject(project) || !projectIds.insert(project.id).second) return false;
    }
    std::unordered_set<std::string> collectionIds;
    for (const auto& collection : collections) {
        if (!validateCollectionShape(collection) || !collectionIds.insert(collection.id).second) return false;
        if (std::any_of(collection.projects.begin(), collection.projects.end(), [&](const auto& id) {
                return projectIds.count(id) == 0U;
            })) return false;
    }
    return true;
}

bool jsonHasOnly(const nlohmann::json& value, const std::set<std::string>& keys) {
    if (!value.is_object() || value.size() != keys.size()) return false;
    for (auto it = value.cbegin(); it != value.cend(); ++it) {
        if (keys.count(it.key()) == 0U) return false;
    }
    return true;
}

bool parseAuthor(const nlohmann::json& j, Author& value) {
    static const std::set<std::string> required = {"github", "name"};
    static const std::set<std::string> withTwitter = {"github", "name", "twitter"};
    if (!jsonHasOnly(j, j.contains("twitter") ? withTwitter : required)) return false;
    try {
        value.name = j.at("name").get<std::string>();
        value.github = j.at("github").get<std::string>();
        value.twitter.reset();
        if (j.contains("twitter")) {
            if (!j.at("twitter").is_string()) return false;
            value.twitter = j.at("twitter").get<std::string>();
        }
        return validateAuthor(value);
    } catch (...) { return false; }
}

bool parseDonation(const nlohmann::json& j, Donation& value) {
    static const std::set<std::string> keys = {"amount", "date", "transactionHash"};
    if (!jsonHasOnly(j, keys)) return false;
    try {
        value.transactionHash = j.at("transactionHash").get<std::string>();
        value.amount = j.at("amount").get<std::string>();
        value.date = j.at("date").get<std::string>();
        return validateDonation(value);
    } catch (...) { return false; }
}

bool parseMetrics(const nlohmann::json& j, Metrics& value) {
    static const std::set<std::string> keys = {"forks", "stars"};
    if (!jsonHasOnly(j, keys)) return false;
    try {
        value.stars = j.at("stars").get<int>();
        value.forks = j.at("forks").get<int>();
        return value.stars >= 0 && value.forks >= 0;
    } catch (...) { return false; }
}

bool parseProject(const nlohmann::json& j, Project& value) {
    static const std::set<std::string> required = {
        "addedOn", "author", "description", "donation", "github", "id", "image",
        "name", "projectUrl", "tags"};
    static const std::set<std::string> withMetrics = {
        "addedOn", "author", "description", "donation", "github", "id", "image",
        "metrics", "name", "projectUrl", "tags"};
    if (!jsonHasOnly(j, j.contains("metrics") ? withMetrics : required)) return false;
    try {
        value.id = j.at("id").get<std::string>();
        value.name = j.at("name").get<std::string>();
        value.description = j.at("description").get<std::string>();
        value.projectUrl = j.at("projectUrl").get<std::string>();
        value.github = j.at("github").get<std::string>();
        value.image = j.at("image").get<std::string>();
        value.tags = j.at("tags").get<std::vector<std::string>>();
        value.addedOn = j.at("addedOn").get<std::string>();
        if (!parseAuthor(j.at("author"), value.author) ||
            !parseDonation(j.at("donation"), value.donation)) return false;
        value.metrics.reset();
        if (j.contains("metrics")) {
            Metrics metrics;
            if (!parseMetrics(j.at("metrics"), metrics)) return false;
            value.metrics = metrics;
        }
        return validateProject(value);
    } catch (...) { return false; }
}

bool parseCurator(const nlohmann::json& j, Curator& value) {
    static const std::set<std::string> keys = {"github", "name"};
    if (!jsonHasOnly(j, keys)) return false;
    try {
        value.name = j.at("name").get<std::string>();
        value.github = j.at("github").get<std::string>();
        return isText(value.name) && isGithub(value.github);
    } catch (...) { return false; }
}

bool parseCollection(const nlohmann::json& j, Collection& value) {
    static const std::set<std::string> keys = {
        "curator", "description", "featured", "id", "name", "projects"};
    if (!jsonHasOnly(j, keys)) return false;
    try {
        value.id = j.at("id").get<std::string>();
        value.name = j.at("name").get<std::string>();
        value.description = j.at("description").get<std::string>();
        value.projects = j.at("projects").get<std::vector<std::string>>();
        value.featured = j.at("featured").get<bool>();
        return parseCurator(j.at("curator"), value.curator) && validateCollectionShape(value);
    } catch (...) { return false; }
}

nlohmann::json authorJson(const Author& value) {
    nlohmann::json result = {{"name", value.name}, {"github", value.github}};
    if (value.twitter) result["twitter"] = *value.twitter;
    return result;
}

nlohmann::json donationJson(const Donation& value) {
    return {{"transactionHash", value.transactionHash}, {"amount", value.amount}, {"date", value.date}};
}

nlohmann::json metricsJson(const Metrics& value) {
    return {{"stars", value.stars}, {"forks", value.forks}};
}

nlohmann::json projectJson(const Project& value) {
    nlohmann::json result = {
        {"id", value.id}, {"name", value.name}, {"description", value.description},
        {"projectUrl", value.projectUrl}, {"github", value.github}, {"image", value.image},
        {"author", authorJson(value.author)}, {"donation", donationJson(value.donation)},
        {"tags", value.tags}, {"addedOn", value.addedOn}};
    if (value.metrics) result["metrics"] = metricsJson(*value.metrics);
    return result;
}

nlohmann::json curatorJson(const Curator& value) {
    return {{"name", value.name}, {"github", value.github}};
}

nlohmann::json collectionJson(const Collection& value) {
    return {{"id", value.id}, {"name", value.name}, {"description", value.description},
            {"projects", value.projects}, {"curator", curatorJson(value.curator)},
            {"featured", value.featured}};
}

bool equalProject(const Project& a, const Project& b) {
    return projectJson(a) == projectJson(b);
}

bool equalCollection(const Collection& a, const Collection& b) {
    return collectionJson(a) == collectionJson(b);
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::filesystem::path normalizedAbsolute(const std::filesystem::path& path) {
    std::error_code ec;
    auto absolute = std::filesystem::absolute(path, ec);
    return ec ? std::filesystem::path{} : absolute.lexically_normal();
}

bool isWithin(const std::filesystem::path& root, const std::filesystem::path& path) {
    auto rootIt = root.begin();
    auto pathIt = path.begin();
    for (; rootIt != root.end(); ++rootIt, ++pathIt) {
        if (pathIt == path.end() || *rootIt != *pathIt) return false;
    }
    return true;
}

bool hasSymlink(const std::filesystem::path& path) {
    std::filesystem::path current;
    for (const auto& part : path) {
        current /= part;
        std::error_code ec;
        const auto status = std::filesystem::symlink_status(current, ec);
        if (!ec && std::filesystem::is_symlink(status)) return true;
    }
    return false;
}

bool resolveConfined(const std::filesystem::path& root, const std::string& filePath,
                     std::filesystem::path& result) {
    if (filePath.empty()) return false;
    const std::filesystem::path requested(filePath);
    result = normalizedAbsolute(requested.is_absolute() ? requested : root / requested);
    return !result.empty() && isWithin(root, result) && !hasSymlink(result.parent_path());
}

bool readBounded(const std::filesystem::path& path, std::string& content) {
    std::error_code ec;
    const auto status = std::filesystem::symlink_status(path, ec);
    if (ec || std::filesystem::is_symlink(status) || !std::filesystem::is_regular_file(status)) return false;
    const auto size = std::filesystem::file_size(path, ec);
    if (ec || size > kMaxSnapshotBytes) return false;
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return file.good() || file.eof();
}

bool writeAtomic(const std::filesystem::path& target, const std::string& content) {
    std::error_code ec;
    std::filesystem::create_directories(target.parent_path(), ec);
    if (ec || hasSymlink(target.parent_path())) return false;
    const auto status = std::filesystem::symlink_status(target, ec);
    if (!ec && std::filesystem::is_symlink(status)) return false;
    ec.clear();
    const auto temporary = target.parent_path() /
        (target.filename().string() + ".tmp." + std::to_string(g_tempCounter.fetch_add(1)));
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        if (!file) return false;
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        file.flush();
        if (!file) {
            file.close();
            std::filesystem::remove(temporary, ec);
            return false;
        }
    }
    if (std::rename(temporary.string().c_str(), target.string().c_str()) != 0) {
        std::filesystem::remove(temporary, ec);
        return false;
    }
    return true;
}

bool parseRoot(const std::string& data, std::vector<Project>& projects,
               std::vector<Collection>& collections, bool projectsOnly) {
    if (data.empty() || data.size() > kMaxSnapshotBytes) return false;
    try {
        const auto json = nlohmann::json::parse(data);
        const std::set<std::string> fullKeys = {"collections", "projects", "schema", "version"};
        const std::set<std::string> projectKeys = {"projects", "schema", "version"};
        if (!jsonHasOnly(json, projectsOnly ? projectKeys : fullKeys) ||
            json.at("schema") != kSchemaName || json.at("version") != kSchemaVersion ||
            !json.at("projects").is_array() || json.at("projects").size() > kMaxProjects) return false;
        projects.clear();
        collections.clear();
        projects.reserve(json.at("projects").size());
        for (const auto& item : json.at("projects")) {
            Project project;
            if (!parseProject(item, project)) return false;
            projects.push_back(std::move(project));
        }
        if (!projectsOnly) {
            if (!json.at("collections").is_array() || json.at("collections").size() > kMaxCollections) return false;
            collections.reserve(json.at("collections").size());
            for (const auto& item : json.at("collections")) {
                Collection collection;
                if (!parseCollection(item, collection)) return false;
                collections.push_back(std::move(collection));
            }
        }
        return validateState(projects, collections);
    } catch (...) { return false; }
}

} // namespace

void to_json(nlohmann::json& j, const Author& a) { j = authorJson(a); }
void from_json(const nlohmann::json& j, Author& a) {
    Author staged;
    if (!parseAuthor(j, staged)) throw nlohmann::json::type_error::create(302, "invalid author", &j);
    a = std::move(staged);
}
void to_json(nlohmann::json& j, const Donation& d) { j = donationJson(d); }
void from_json(const nlohmann::json& j, Donation& d) {
    Donation staged;
    if (!parseDonation(j, staged)) throw nlohmann::json::type_error::create(302, "invalid donation", &j);
    d = std::move(staged);
}
void to_json(nlohmann::json& j, const Metrics& m) { j = metricsJson(m); }
void from_json(const nlohmann::json& j, Metrics& m) {
    Metrics staged;
    if (!parseMetrics(j, staged)) throw nlohmann::json::type_error::create(302, "invalid metrics", &j);
    m = staged;
}
void to_json(nlohmann::json& j, const Project& p) { j = projectJson(p); }
void from_json(const nlohmann::json& j, Project& p) {
    Project staged;
    if (!parseProject(j, staged)) throw nlohmann::json::type_error::create(302, "invalid project", &j);
    p = std::move(staged);
}
void to_json(nlohmann::json& j, const Curator& c) { j = curatorJson(c); }
void from_json(const nlohmann::json& j, Curator& c) {
    Curator staged;
    if (!parseCurator(j, staged)) throw nlohmann::json::type_error::create(302, "invalid curator", &j);
    c = std::move(staged);
}
void to_json(nlohmann::json& j, const Collection& c) { j = collectionJson(c); }
void from_json(const nlohmann::json& j, Collection& c) {
    Collection staged;
    if (!parseCollection(j, staged)) throw nlohmann::json::type_error::create(302, "invalid collection", &j);
    c = std::move(staged);
}

ElizasList::ElizasList() : ElizasList(std::filesystem::current_path()) {}

ElizasList::ElizasList(std::filesystem::path persistenceRoot)
    : persistenceRoot_(normalizedAbsolute(persistenceRoot)) {
    if (persistenceRoot_.empty()) throw std::invalid_argument("invalid persistence root");
}

bool ElizasList::addProject(const Project& project) {
    if (!validateProject(project)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = std::find_if(projects_.begin(), projects_.end(), [&](const auto& value) {
        return value.id == project.id;
    });
    if (found != projects_.end()) return equalProject(*found, project);
    projects_.push_back(project);
    return true;
}

bool ElizasList::removeProject(const std::string& projectId) {
    if (!isIdentifier(projectId)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (std::any_of(collections_.begin(), collections_.end(), [&](const auto& collection) {
            return std::find(collection.projects.begin(), collection.projects.end(), projectId) !=
                   collection.projects.end();
        })) return false;
    const auto found = std::find_if(projects_.begin(), projects_.end(), [&](const auto& value) {
        return value.id == projectId;
    });
    if (found == projects_.end()) return false;
    projects_.erase(found);
    return true;
}

std::optional<Project> ElizasList::getProject(const std::string& projectId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = std::find_if(projects_.begin(), projects_.end(), [&](const auto& value) {
        return value.id == projectId;
    });
    return found == projects_.end() ? std::nullopt : std::optional<Project>(*found);
}

std::vector<Project> ElizasList::getAllProjects() const {
    std::vector<Project> result;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        result = projects_;
    }
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.id < b.id; });
    return result;
}

std::vector<Project> ElizasList::getProjectsByTag(const std::string& tag) const {
    auto result = getAllProjects();
    result.erase(std::remove_if(result.begin(), result.end(), [&](const auto& project) {
        return std::find(project.tags.begin(), project.tags.end(), tag) == project.tags.end();
    }), result.end());
    return result;
}

std::vector<Project> ElizasList::getProjectsByAuthor(const std::string& authorGithub) const {
    auto result = getAllProjects();
    result.erase(std::remove_if(result.begin(), result.end(), [&](const auto& project) {
        return project.author.github != authorGithub;
    }), result.end());
    return result;
}

bool ElizasList::updateProject(const Project& project) {
    if (!validateProject(project)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = std::find_if(projects_.begin(), projects_.end(), [&](const auto& value) {
        return value.id == project.id;
    });
    if (found == projects_.end()) return false;
    *found = project;
    return true;
}

bool ElizasList::addCollection(const Collection& collection) {
    if (!validateCollectionShape(collection)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = std::find_if(collections_.begin(), collections_.end(), [&](const auto& value) {
        return value.id == collection.id;
    });
    if (found != collections_.end()) return equalCollection(*found, collection);
    if (std::any_of(collection.projects.begin(), collection.projects.end(), [&](const auto& id) {
            return std::none_of(projects_.begin(), projects_.end(), [&](const auto& project) {
                return project.id == id;
            });
        })) return false;
    collections_.push_back(collection);
    return true;
}

bool ElizasList::removeCollection(const std::string& collectionId) {
    if (!isIdentifier(collectionId)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = std::find_if(collections_.begin(), collections_.end(), [&](const auto& value) {
        return value.id == collectionId;
    });
    if (found == collections_.end()) return false;
    collections_.erase(found);
    return true;
}

std::optional<Collection> ElizasList::getCollection(const std::string& collectionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = std::find_if(collections_.begin(), collections_.end(), [&](const auto& value) {
        return value.id == collectionId;
    });
    return found == collections_.end() ? std::nullopt : std::optional<Collection>(*found);
}

std::vector<Collection> ElizasList::getAllCollections() const {
    std::vector<Collection> result;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        result = collections_;
    }
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.id < b.id; });
    return result;
}

std::vector<Collection> ElizasList::getFeaturedCollections() const {
    auto result = getAllCollections();
    result.erase(std::remove_if(result.begin(), result.end(), [](const auto& collection) {
        return !collection.featured;
    }), result.end());
    return result;
}

bool ElizasList::updateCollection(const Collection& collection) {
    if (!validateCollectionShape(collection)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = std::find_if(collections_.begin(), collections_.end(), [&](const auto& value) {
        return value.id == collection.id;
    });
    if (found == collections_.end()) return false;
    if (std::any_of(collection.projects.begin(), collection.projects.end(), [&](const auto& id) {
            return std::none_of(projects_.begin(), projects_.end(), [&](const auto& project) {
                return project.id == id;
            });
        })) return false;
    *found = collection;
    return true;
}

std::vector<Project> ElizasList::searchProjects(const std::string& query) const {
    const auto needle = lower(query);
    auto result = getAllProjects();
    result.erase(std::remove_if(result.begin(), result.end(), [&](const auto& project) {
        if (needle.empty()) return false;
        if (lower(project.id).find(needle) != std::string::npos ||
            lower(project.name).find(needle) != std::string::npos ||
            lower(project.description).find(needle) != std::string::npos ||
            lower(project.author.name).find(needle) != std::string::npos ||
            lower(project.author.github).find(needle) != std::string::npos) return false;
        return std::none_of(project.tags.begin(), project.tags.end(), [&](const auto& tag) {
            return lower(tag).find(needle) != std::string::npos;
        });
    }), result.end());
    return result;
}

std::vector<Project> ElizasList::getProjectsSortedByStars() const {
    auto result = getAllProjects();
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        const int aStars = a.metrics ? a.metrics->stars : 0;
        const int bStars = b.metrics ? b.metrics->stars : 0;
        return aStars == bStars ? a.id < b.id : aStars > bStars;
    });
    return result;
}

std::vector<Project> ElizasList::getRecentProjects(int limit) const {
    if (limit <= 0) return {};
    auto result = getAllProjects();
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        return a.addedOn == b.addedOn ? a.id < b.id : a.addedOn > b.addedOn;
    });
    if (static_cast<std::size_t>(limit) < result.size()) result.resize(static_cast<std::size_t>(limit));
    return result;
}

bool ElizasList::loadFromJson(const std::string& filePath) {
    std::filesystem::path target;
    if (!resolveConfined(persistenceRoot_, filePath, target) || hasSymlink(target)) return false;
    std::string content;
    std::vector<Project> projects;
    std::vector<Collection> collections;
    if (!readBounded(target, content) || !parseRoot(content, projects, collections, false)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    projects_.swap(projects);
    collections_.swap(collections);
    return true;
}

bool ElizasList::saveToJson(const std::string& filePath) const {
    std::filesystem::path target;
    if (!resolveConfined(persistenceRoot_, filePath, target)) return false;
    std::vector<Project> projects;
    std::vector<Collection> collections;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        projects = projects_;
        collections = collections_;
    }
    std::sort(projects.begin(), projects.end(), [](const auto& a, const auto& b) {
        return a.id < b.id;
    });
    std::sort(collections.begin(), collections.end(), [](const auto& a, const auto& b) {
        return a.id < b.id;
    });
    nlohmann::json json = {{"schema", kSchemaName}, {"version", kSchemaVersion},
                           {"projects", nlohmann::json::array()},
                           {"collections", nlohmann::json::array()}};
    for (const auto& project : projects) json["projects"].push_back(projectJson(project));
    for (const auto& collection : collections) json["collections"].push_back(collectionJson(collection));
    return writeAtomic(target, json.dump(2));
}

bool ElizasList::loadProjectsFromJson(const std::string& jsonData) {
    std::vector<Project> projects;
    std::vector<Collection> ignored;
    if (!parseRoot(jsonData, projects, ignored, true)) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (std::any_of(collections_.begin(), collections_.end(), [&](const auto& collection) {
            return std::any_of(collection.projects.begin(), collection.projects.end(), [&](const auto& id) {
                return std::none_of(projects.begin(), projects.end(), [&](const auto& project) {
                    return project.id == id;
                });
            });
        })) return false;
    projects_.swap(projects);
    return true;
}

std::string ElizasList::exportProjectsToJson() const {
    nlohmann::json json = {{"schema", kSchemaName}, {"version", kSchemaVersion},
                           {"projects", nlohmann::json::array()}};
    for (const auto& project : getAllProjects()) json["projects"].push_back(projectJson(project));
    return json.dump(2);
}

std::filesystem::path ElizasList::getPersistenceRoot() const { return persistenceRoot_; }

std::size_t ElizasList::getProjectCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return projects_.size();
}

std::size_t ElizasList::getCollectionCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return collections_.size();
}

std::vector<std::string> ElizasList::getAllTags() const {
    std::set<std::string> tags;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& project : projects_) tags.insert(project.tags.begin(), project.tags.end());
    }
    return {tags.begin(), tags.end()};
}

} // namespace elizaos
