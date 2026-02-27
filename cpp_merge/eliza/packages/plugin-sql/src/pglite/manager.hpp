#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC_PGLITE_MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC_PGLITE_MANAGER_H
#include "core.hpp"
// External dependency removed
// External dependency removed
// External dependency removed
#include "../types.h"

class PGliteClientManager;

class PGliteClientManager : public IDatabaseClientManager<PGlite>, public std::enable_shared_from_this<PGliteClientManager> {
public:
    using std::enable_shared_from_this<PGliteClientManager>::shared_from_this;
    std::shared_ptr<PGlite> client;

    boolean shuttingDown = false;

    PGliteClientManager(std::shared_ptr<PGliteOptions> options);
    virtual std::shared_ptr<PGlite> getConnection();
    virtual boolean isShuttingDown();
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<void>> close();
    virtual void setupShutdownHandlers();
};

#endif
