#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_TOKENSUPPLYHELPERS_MONITORING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_TOKENSUPPLYHELPERS_MONITORING_H
#include "core.h"
#include "drizzle-orm.h"
#include "../db.h"
#include "../externalToken.h"
#include "../redis.h"
#include "../util.h"

std::shared_ptr<Promise<object>> startMonitoringBatch(double batchSize = 10);

#endif
