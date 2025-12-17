#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PackageInfo {
    std::string name;
    std::string version;
    std::string description;
    double downloads;
    std::string repository;
    std::vector<std::string> dependencies;
};

class PackageManagerClient {
public:
    static std::future<PackageInfo> getNpmInfo(const std::string& packageName);
    static std::future<PackageInfo> getPyPiInfo(const std::string& packageName);
};
 
} // namespace elizaos
