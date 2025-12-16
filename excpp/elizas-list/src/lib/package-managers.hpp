#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

struct PackageInfo {
    std::string name;
    std::string version;
    std::string description;
    double downloads;
    std::string repository;
    std::vector<std::string> dependencies;
};


class PackageManagerClient {
  static async getNpmInfo(packageName: string): Promise<PackageInfo> {
    const response = await axios.get(`https://registry.npmjs.org/${packageName}`);
    const latest = response.data['dist-tags'].latest;
    const info = response.data.versions[latest];
    
    const downloads = await axios.get(
      `https://api.npmjs.org/downloads/point/last-month/${packageName}`
    );

    return {
      name: packageName,
      version: latest,
      description: info.description,
      downloads: downloads.data.downloads,
      repository: info.repository?.url,
      dependencies: Object.keys(info.dependencies || {}),
    };
  }

  static async getPyPiInfo(packageName: string): Promise<PackageInfo> {
    const response = await axios.get(`https://pypi.org/pypi/${packageName}/json`);
    const info = response.data.info;
    
    return {
      name: packageName,
      version: info.version,
      description: info.summary,
      downloads: 0, // PyPI doesn't provide download stats
      repository: info.project_urls?.Repository,
      dependencies: [], // Would need to parse requirements.txt
    };
  }
} 
} // namespace elizaos
