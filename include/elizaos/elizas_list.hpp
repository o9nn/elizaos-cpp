#pragma once

#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace elizaos {

struct Author {
    std::string name;
    std::string github;
    std::optional<std::string> twitter;
};

struct Donation {
    std::string transactionHash;
    std::string amount;
    std::string date;
};

struct Metrics {
    int stars = 0;
    int forks = 0;
};

struct Project {
    std::string id;
    std::string name;
    std::string description;
    std::string projectUrl;
    std::string github;
    std::string image;
    Author author;
    Donation donation;
    std::vector<std::string> tags;
    std::string addedOn;
    std::optional<Metrics> metrics;
};

struct Curator {
    std::string name;
    std::string github;
};

struct Collection {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> projects;
    Curator curator;
    bool featured = false;
};

void to_json(nlohmann::json& j, const Author& a);
void from_json(const nlohmann::json& j, Author& a);
void to_json(nlohmann::json& j, const Donation& d);
void from_json(const nlohmann::json& j, Donation& d);
void to_json(nlohmann::json& j, const Metrics& m);
void from_json(const nlohmann::json& j, Metrics& m);
void to_json(nlohmann::json& j, const Project& p);
void from_json(const nlohmann::json& j, Project& p);
void to_json(nlohmann::json& j, const Curator& c);
void from_json(const nlohmann::json& j, Curator& c);
void to_json(nlohmann::json& j, const Collection& c);
void from_json(const nlohmann::json& j, Collection& c);

class ElizasList {
public:
    ElizasList();
    explicit ElizasList(std::filesystem::path persistenceRoot);
    ~ElizasList() = default;

    ElizasList(const ElizasList&) = delete;
    ElizasList& operator=(const ElizasList&) = delete;

    bool addProject(const Project& project);
    bool removeProject(const std::string& projectId);
    std::optional<Project> getProject(const std::string& projectId) const;
    std::vector<Project> getAllProjects() const;
    std::vector<Project> getProjectsByTag(const std::string& tag) const;
    std::vector<Project> getProjectsByAuthor(const std::string& authorGithub) const;
    bool updateProject(const Project& project);

    bool addCollection(const Collection& collection);
    bool removeCollection(const std::string& collectionId);
    std::optional<Collection> getCollection(const std::string& collectionId) const;
    std::vector<Collection> getAllCollections() const;
    std::vector<Collection> getFeaturedCollections() const;
    bool updateCollection(const Collection& collection);

    std::vector<Project> searchProjects(const std::string& query) const;
    std::vector<Project> getProjectsSortedByStars() const;
    std::vector<Project> getRecentProjects(int limit = 10) const;

    bool loadFromJson(const std::string& filePath);
    bool saveToJson(const std::string& filePath) const;
    bool loadProjectsFromJson(const std::string& jsonData);
    std::string exportProjectsToJson() const;
    std::filesystem::path getPersistenceRoot() const;

    size_t getProjectCount() const;
    size_t getCollectionCount() const;
    std::vector<std::string> getAllTags() const;

private:
    std::vector<Project> projects_;
    std::vector<Collection> collections_;
    std::filesystem::path persistenceRoot_;
    mutable std::mutex mutex_;
};

} // namespace elizaos
