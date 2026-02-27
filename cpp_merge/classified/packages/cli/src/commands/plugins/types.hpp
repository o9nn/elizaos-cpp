#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PLUGINS_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PLUGINS_TYPES_H
#include "core.hpp"

typedef Record<string, string> Dependencies;

class ListPluginsOptions;
class AddPluginOptions;
class UpgradePluginOptions;
class GeneratePluginOptions;
class PluginRegistryInfo;
class PluginRegistry;
class EnvVarConfig;
class MigrationResult;
class GenerationResult;
class DirectoryInfo;

class ListPluginsOptions : public object, public std::enable_shared_from_this<ListPluginsOptions> {
public:
    using std::enable_shared_from_this<ListPluginsOptions>::shared_from_this;
    boolean all;

    boolean v0;
};

class AddPluginOptions : public object, public std::enable_shared_from_this<AddPluginOptions> {
public:
    using std::enable_shared_from_this<AddPluginOptions>::shared_from_this;
    boolean skipEnvPrompt;

    boolean skipVerification;

    string branch;

    string tag;
};

class UpgradePluginOptions : public object, public std::enable_shared_from_this<UpgradePluginOptions> {
public:
    using std::enable_shared_from_this<UpgradePluginOptions>::shared_from_this;
    string apiKey;

    boolean skipTests;

    boolean skipValidation;

    boolean quiet;

    boolean verbose;

    boolean debug;

    boolean skipConfirmation;
};

class GeneratePluginOptions : public object, public std::enable_shared_from_this<GeneratePluginOptions> {
public:
    using std::enable_shared_from_this<GeneratePluginOptions>::shared_from_this;
    string apiKey;

    boolean skipTests;

    boolean skipValidation;

    boolean skipPrompts;

    string specFile;
};

class PluginRegistryInfo : public object, public std::enable_shared_from_this<PluginRegistryInfo> {
public:
    using std::enable_shared_from_this<PluginRegistryInfo>::shared_from_this;
    object git;

    object npm;

    object supports;
};

class PluginRegistry : public object, public std::enable_shared_from_this<PluginRegistry> {
public:
    using std::enable_shared_from_this<PluginRegistry>::shared_from_this;
    Record<string, std::shared_ptr<PluginRegistryInfo>> registry;
};

class EnvVarConfig : public object, public std::enable_shared_from_this<EnvVarConfig> {
public:
    using std::enable_shared_from_this<EnvVarConfig>::shared_from_this;
    string type;

    string description;

    boolean required;

    string default;

    boolean sensitive;
};

class MigrationResult : public object, public std::enable_shared_from_this<MigrationResult> {
public:
    using std::enable_shared_from_this<MigrationResult>::shared_from_this;
    boolean success;

    string branchName;

    string repoPath;

    std::shared_ptr<Error> error;
};

class GenerationResult : public object, public std::enable_shared_from_this<GenerationResult> {
public:
    using std::enable_shared_from_this<GenerationResult>::shared_from_this;
    boolean success;

    string pluginName;

    string pluginPath;

    std::shared_ptr<Error> error;
};

class DirectoryInfo : public object, public std::enable_shared_from_this<DirectoryInfo> {
public:
    using std::enable_shared_from_this<DirectoryInfo>::shared_from_this;
    string type;

    boolean hasPackageJson;
};

#endif
