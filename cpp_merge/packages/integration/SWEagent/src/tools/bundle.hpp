#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_TOOLS_BUNDLE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_TOOLS_BUNDLE_H
#include "core.hpp"
#include "path.hpp"
// Using alias removed (invalid transpilation)
#include "fs.hpp"
// Using alias removed (invalid transpilation)
#include "js-yaml.h"
// Using alias removed (invalid transpilation)
#include "./commands.h"

class BundleConfig;
class Bundle;

class BundleConfig : public object, public std::enable_shared_from_this<BundleConfig> {
public:
    using std::enable_shared_from_this<BundleConfig>::shared_from_this;
    Record<string, any> tools;

    any stateCommand;
};

class Bundle : public object, public std::enable_shared_from_this<Bundle> {
public:
    using std::enable_shared_from_this<Bundle>::shared_from_this;
    string path;

    array<string> hiddenTools;

    std::shared_ptr<BundleConfig> _config;

    Bundle(object config);
    virtual void validateTools();
    virtual any get_stateCommand();
    virtual std::shared_ptr<BundleConfig> get_config();
    virtual array<std::shared_ptr<Command>> get_commands();
};

#endif
