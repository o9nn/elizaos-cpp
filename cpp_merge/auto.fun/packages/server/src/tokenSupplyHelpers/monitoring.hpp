#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_TOKENSUPPLYHELPERS_MONITORING_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_TOKENSUPPLYHELPERS_MONITORING_H
#include "core.hpp"
#include "drizzle-orm.h"
#include "../db.h"
#include "../externalToken.h"
#include "../redis.h"
#include "../util.h"

std::shared_ptr<Promise<object>> startMonitoringBatch(double batchSize = 10);

#endif
