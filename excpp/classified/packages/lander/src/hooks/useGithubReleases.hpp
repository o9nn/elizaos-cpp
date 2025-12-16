#include ".utils/repository.hpp"
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
;

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
    'macOS' | 'Windows' | 'Linux' platform;
    std::optional<'x64' | 'arm64' | 'universal'> architecture;
    std::string filename;
    std::string size;
    std::string downloadUrl;
    'installer' | 'portable' | 'dmg' | 'appimage' | 'deb' type;
};


const useGithubReleases = () => {
  const [releases] = useState<GitHubRelease[]>([]);
  const [latestRelease, setLatestRelease] = useState<GitHubRelease | null>(null);
  const [downloadLinks, setDownloadLinks] = useState<DownloadLink[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  const formatFileSize = (bytes: number): string => {
    const units = ['B', 'KB', 'MB', 'GB'];
    let size = bytes;
    let unitIndex = 0;

    while (size >= 1024 && unitIndex < units.length - 1) {
      size /= 1024;
      unitIndex++;
    }

    return `${size.toFixed(1)} ${units[unitIndex]}`;
  };

  const parseDownloadLinks = (assets: GitHubAsset[]): DownloadLink[] => {
    const links: DownloadLink[] = [];

    assets.forEach((asset) => {
      const filename = asset.name.toLowerCase();

      // macOS
      if (filename.includes('.dmg')) {
        links.push({
          platform: 'macOS',
          architecture: filename.includes('universal')
            ? 'universal'
            : filename.includes('arm64')
              ? 'arm64'
              : 'x64',
          filename: asset.name,
          size: formatFileSize(asset.size),
          downloadUrl: asset.browser_download_url,
          type: 'dmg',
        });
      }

      // Windows
      else if (filename.includes('.exe') || filename.includes('.msi')) {
        links.push({
          platform: 'Windows',
          architecture: filename.includes('arm64') ? 'arm64' : 'x64',
          filename: asset.name,
          size: formatFileSize(asset.size),
          downloadUrl: asset.browser_download_url,
          type: 'installer',
        });
      }

      // Linux AppImage
      else if (filename.includes('.appimage')) {
        links.push({
          platform: 'Linux',
          architecture: filename.includes('arm64') ? 'arm64' : 'x64',
          filename: asset.name,
          size: formatFileSize(asset.size),
          downloadUrl: asset.browser_download_url,
          type: 'appimage',
        });
      }

      // Linux DEB
      else if (filename.includes('.deb')) {
        links.push({
          platform: 'Linux',
          architecture: filename.includes('arm64') ? 'arm64' : 'x64',
          filename: asset.name,
          size: formatFileSize(asset.size),
          downloadUrl: asset.browser_download_url,
          type: 'deb',
        });
      }
    });

    return links;
  };

  const fetchReleases = async () => {
    setLoading(true);
    setError(null);

    try {
      const response = await fetch(getApiReleasesUrl());

      if (!response.ok) {
        throw new Error(`GitHub API error: ${response.status}`);
      }

      const data: GitHubRelease[] = await response.json();

      // Find the latest non-prerelease version
      const latestStable = data.find((release) => !release.prerelease && !release.draft);

      if (latestStable) {
        setLatestRelease(latestStable);
        const links = parseDownloadLinks(latestStable.assets);
        setDownloadLinks(links);
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to fetch releases');
    } finally {
      setLoading(false);
    }
  };

  const refetch = () => {
    fetchReleases();
  };

  useEffect(() => {
    fetchReleases();
  }, []);

  return {
    releases,
    latestRelease,
    downloadLinks,
    loading,
    error,
    refetch,
  };
};

} // namespace elizaos
