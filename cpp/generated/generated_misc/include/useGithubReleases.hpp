#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_LANDER_SRC_HOOKS_USEGITHUBRELEASES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_LANDER_SRC_HOOKS_USEGITHUBRELEASES_H
#include "core.h"
#include "react.h"
#include "../utils/repository.h"

class GitHubAsset;
class GitHubRelease;
class DownloadLink;

class GitHubAsset : public object, public std::enable_shared_from_this<GitHubAsset> {
public:
    using std::enable_shared_from_this<GitHubAsset>::shared_from_this;
    double id;

    std::string name;

    std::string label;

    std::string content_type;

    double size;

    double download_count;

    std::string browser_download_url;

    std::string created_at;

    std::string updated_at;
};

class GitHubRelease : public object, public std::enable_shared_from_this<GitHubRelease> {
public:
    using std::enable_shared_from_this<GitHubRelease>::shared_from_this;
    double id;

    std::string tag_name;

    std::string name;

    std::string body;

    boolean draft;

    boolean prerelease;

    std::string created_at;

    std::string published_at;

    array<std::shared_ptr<GitHubAsset>> assets;

    std::string html_url;

    std::string tarball_url;

    std::string zipball_url;
};

class DownloadLink : public object, public std::enable_shared_from_this<DownloadLink> {
public:
    using std::enable_shared_from_this<DownloadLink>::shared_from_this;
    std::any platform;

    std::any architecture;

    std::string filename;

    std::string size;

    std::string downloadUrl;

    std::any type;
};

extern std::function<object()> useGithubReleases;
#endif
