#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct Project {
    std::string id;
    std::string name;
    std::string description;
    std::string projectUrl;
    std::string github;
    std::string image;
    std::string name;
    std::string github;
    std::optional<std::string> twitter;
    std::string transactionHash;
    std::string amount;
    std::string date;
    std::vector<std::string> tags;
    std::string addedOn;
    double stars;
    double forks;
};

struct Collection {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> projects;
    std::string name;
    std::string github;
    bool featured;
};
 
} // namespace elizaos
