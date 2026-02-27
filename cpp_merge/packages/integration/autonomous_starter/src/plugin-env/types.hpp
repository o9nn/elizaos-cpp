#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ENV_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ENV_TYPES_H
#include "core.hpp"

class EnvVarConfig;
class EnvVarMetadata;
class GenerationScript;
class GenerationScriptMetadata;
class EnvVarUpdate;
class ValidationResult;

class EnvVarConfig : public object, public std::enable_shared_from_this<EnvVarConfig> {
public:
    using std::enable_shared_from_this<EnvVarConfig>::shared_from_this;
    string value;

    any type;

    boolean required;

    string description;

    boolean canGenerate;

    string validationMethod;

    any status;

    string lastError;

    double attempts;

    double createdAt;

    double validatedAt;

    string plugin;
};

class EnvVarMetadata : public object, public std::enable_shared_from_this<EnvVarMetadata> {
public:
    using std::enable_shared_from_this<EnvVarMetadata>::shared_from_this;
};

class GenerationScript : public object, public std::enable_shared_from_this<GenerationScript> {
public:
    using std::enable_shared_from_this<GenerationScript>::shared_from_this;
    string variableName;

    string pluginName;

    string script;

    array<string> dependencies;

    double attempts;

    string output;

    string error;

    any status;

    double createdAt;
};

class GenerationScriptMetadata : public object, public std::enable_shared_from_this<GenerationScriptMetadata> {
public:
    using std::enable_shared_from_this<GenerationScriptMetadata>::shared_from_this;
};

class EnvVarUpdate : public object, public std::enable_shared_from_this<EnvVarUpdate> {
public:
    using std::enable_shared_from_this<EnvVarUpdate>::shared_from_this;
    string pluginName;

    string variableName;

    string value;
};

class ValidationResult : public object, public std::enable_shared_from_this<ValidationResult> {
public:
    using std::enable_shared_from_this<ValidationResult>::shared_from_this;
    boolean isValid;

    string error;

    string details;
};

#endif
