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

    string name;

    string label;

    string content_type;

    double size;

    double download_count;

    string browser_download_url;

    string created_at;

    string updated_at;
};

class GitHubRelease : public object, public std::enable_shared_from_this<GitHubRelease> {
public:
    using std::enable_shared_from_this<GitHubRelease>::shared_from_this;
    double id;

    string tag_name;

    string name;

    string body;

    boolean draft;

    boolean prerelease;

    string created_at;

    string published_at;

    array<std::shared_ptr<GitHubAsset>> assets;

    string html_url;

    string tarball_url;

    string zipball_url;
};

class DownloadLink : public object, public std::enable_shared_from_this<DownloadLink> {
public:
    using std::enable_shared_from_this<DownloadLink>::shared_from_this;
    any platform;

    any architecture;

    string filename;

    string size;

    string downloadUrl;

    any type;
};

extern std::function<object()> useGithubReleases;
#endif
