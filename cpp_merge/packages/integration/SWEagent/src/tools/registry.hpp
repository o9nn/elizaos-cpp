#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_TOOLS_REGISTRY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_TOOLS_REGISTRY_H
#include "core.hpp"
#include "fs.hpp"

class EnvRegistry;

class EnvRegistry : public object, public std::enable_shared_from_this<EnvRegistry> {
public:
    using std::enable_shared_from_this<EnvRegistry>::shared_from_this;
    Record<string, any> data = object{};

    any envFile;

    EnvRegistry();
    virtual void load();
    virtual void save();
    virtual any get(string key);
    virtual void set(string key, any value);
    virtual boolean has(string key);
    virtual void delete(string key);
    virtual void clear();
};

extern std::shared_ptr<EnvRegistry> registry;
#endif
