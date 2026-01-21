#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TOOLS_REGISTRY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TOOLS_REGISTRY_H
#include "core.h"
#include "fs.h"

class EnvRegistry;

class EnvRegistry : public object, public std::enable_shared_from_this<EnvRegistry> {
public:
    using std::enable_shared_from_this<EnvRegistry>::shared_from_this;
    Record<std::string, any> data = object{};

    std::any envFile;

    EnvRegistry();
    virtual void load();
    virtual void save();
    virtual std::any get(std::string key);
    virtual void std::set(std::string key, std::any value);
    virtual boolean has(std::string key);
    virtual void delete(std::string key);
    virtual void clear();
};

extern std::shared_ptr<EnvRegistry> registry;
#endif
