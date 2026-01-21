#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_MANAGERS_BUILD-MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_MANAGERS_BUILD-MANAGER_H
#include "core.h"
#include "@elizaos/core.h"
#include "../service.h"
#include "../hyperfy/src/core/utils.js.h"
#include "lodash-es.h"
#include "../hyperfy/src/core/extras/appTools.js.h"
#include "../hyperfy/src/core/utils-client.js.h"
#include "../utils.js.h"

class BuildManager;

class BuildManager : public object, public std::enable_shared_from_this<BuildManager> {
public:
    using std::enable_shared_from_this<BuildManager>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    BuildManager(std::shared_ptr<IAgentRuntime> runtime);
    virtual void translate(std::any entityId, std::tuple<double, double, double> position);
    virtual void rotate(std::any entityId, std::tuple<double, double, double, double> quaternion);
    virtual void scale(std::any entityId, std::tuple<double, double, double> scale);
    virtual void duplicate(std::any entityId);
    virtual void delete(std::any entityId);
    virtual void importEntity(std::string url, std::any position = undefined, std::any quaternion = undefined);
    virtual void addApp(std::any file, std::any transform);
    virtual void addModel(std::any file, std::any transform);
    virtual void entityUpdate(std::any entity);
    virtual std::any getService();
};

#endif
