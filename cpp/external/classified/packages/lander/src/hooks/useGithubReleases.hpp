#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "utils/repository.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct GitHubAsset {
    double id;
    std::string name;
    std::optional<std::string> label;
    std::string content_type;
    double size;
    double download_count;
    std::string browser_download_url;
    std::string created_at;
    std::string updated_at;
};

struct GitHubRelease {
    double id;
    std::string tag_name;
    std::string name;
    std::string body;
    bool draft;
    bool prerelease;
    std::string created_at;
    std::string published_at;
    std::vector<GitHubAsset> assets;
    std::string html_url;
    std::string tarball_url;
    std::string zipball_url;
};

struct DownloadLink {
    std::string platform;
    std::optional<std::string> architecture;
    std::string filename;
    std::string size;
    std::string downloadUrl;
    std::string type;
};

      // macOS

      // Windows

      // Linux AppImage

      // Linux DEB

      // Find the latest non-prerelease version


} // namespace elizaos
