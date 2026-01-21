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
    virtual void translate(any entityId, std::tuple<double, double, double> position);
    virtual void rotate(any entityId, std::tuple<double, double, double, double> quaternion);
    virtual void scale(any entityId, std::tuple<double, double, double> scale);
    virtual void duplicate(any entityId);
    virtual void delete(any entityId);
    virtual void importEntity(string url, any position = undefined, any quaternion = undefined);
    virtual void addApp(any file, any transform);
    virtual void addModel(any file, any transform);
    virtual void entityUpdate(any entity);
    virtual any getService();
};

#endif
