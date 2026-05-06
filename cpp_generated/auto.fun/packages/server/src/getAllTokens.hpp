#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_GETALLTOKENS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_GETALLTOKENS_H
#include "core.h"
#include "@solana/web3.js.h"
#include "drizzle-orm.h"
#include "p-queue.h"
using PQueue = _default;
#include "./db.h"
#include "./logger.h"
#include "./cron.h"

std::shared_ptr<Promise<any>> getLastProcessedSlot();

std::shared_ptr<Promise<void>> setLastProcessedSlot(double slot);

std::shared_ptr<Promise<double>> findSlotAtOrBeforeTime(std::shared_ptr<Connection> connection, double targetTs, double low, double high);

any processSlot(double slot, std::shared_ptr<Connection> connection);

std::shared_ptr<Promise<void>> processMissedEvents(std::shared_ptr<Connection> connection);

#endif
