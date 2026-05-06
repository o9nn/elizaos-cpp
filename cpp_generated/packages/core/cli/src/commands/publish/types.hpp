#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_TYPES_H
#include "core.h"

typedef any PublishResult;

class PackageMetadata;
class PublishOptions;
class Credentials;
class DirectoryInfo;
class RegistrySettings;
class PackageJson;
class PlaceholderReplacement;

class PackageMetadata : public object, public std::enable_shared_from_this<PackageMetadata> {
public:
    using std::enable_shared_from_this<PackageMetadata>::shared_from_this;
    string name;

    string version;

    string description;

    string type;

    string platform;

    string runtimeVersion;

    string repository;

    array<string> maintainers;

    string publishedAt;

    string publishedBy;

    Record<string, string> dependencies;

    array<string> tags;

    string license;

    string npmPackage;

    string githubRepo;
};

class PublishOptions : public object, public std::enable_shared_from_this<PublishOptions> {
public:
    using std::enable_shared_from_this<PublishOptions>::shared_from_this;
    boolean npm;

    boolean test;

    boolean dryRun;

    boolean skipRegistry;
};

class Credentials : public object, public std::enable_shared_from_this<Credentials> {
public:
    using std::enable_shared_from_this<Credentials>::shared_from_this;
    string username;

    string token;
};

class DirectoryInfo : public object, public std::enable_shared_from_this<DirectoryInfo> {
public:
    using std::enable_shared_from_this<DirectoryInfo>::shared_from_this;
    string type;

    boolean hasPackageJson;
};

class RegistrySettings : public object, public std::enable_shared_from_this<RegistrySettings> {
public:
    using std::enable_shared_from_this<RegistrySettings>::shared_from_this;
    string defaultRegistry;

    object publishConfig;
};

class PackageJson : public object, public std::enable_shared_from_this<PackageJson> {
public:
    using std::enable_shared_from_this<PackageJson>::shared_from_this;
    string name;

    string version;

    string description;

    string type;

    any platform;

    object repository;

    array<string> maintainers;

    Record<string, string> dependencies;

    array<string> keywords;

    string license;

    string author;

    object bugs;

    string npmPackage;

    string githubRepo;

    any packageType;

    object agentConfig;

    object eliza;
};

class PlaceholderReplacement : public object, public std::enable_shared_from_this<PlaceholderReplacement> {
public:
    using std::enable_shared_from_this<PlaceholderReplacement>::shared_from_this;
    std::function<boolean()> check;

    std::function<void()> replace;
};

#endif
