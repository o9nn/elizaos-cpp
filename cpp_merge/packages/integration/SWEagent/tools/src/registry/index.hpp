#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TOOLS_SRC_REGISTRY_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TOOLS_SRC_REGISTRY_INDEX_H
#include "core.hpp"
#include "fs.hpp"
#include "path.hpp"
#include "os.hpp"
#include "commander.hpp"

class EnvRegistry;

class EnvRegistry : public object, public std::enable_shared_from_this<EnvRegistry> {
public:
    using std::enable_shared_from_this<EnvRegistry>::shared_from_this;
    string envFile;

    Record<string, any> data = object{};

    EnvRegistry(string envFile = undefined);
    virtual void loadData();
    virtual void saveData();
    virtual any get(string key, any defaultValue = string_empty, boolean fallbackToEnv = true);
    virtual void set(string key, any value);
    virtual void delete(string key);
    virtual Record<string, any> getAll();
};

extern std::shared_ptr<EnvRegistry> registry;
void setupCLI();

#endif
