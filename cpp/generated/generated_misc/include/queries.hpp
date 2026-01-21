#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_QUERIES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_QUERIES_H
#include "core.h"
#include "server-only.h"
#include "@/lib/data/db.h"
#include "@/lib/data/schema.h"
#include "drizzle-orm.h"
#include "@/lib/walletLinking/chainUtils.h"
#include "./readmeUtils.h"

std::shared_ptr<Promise<any>> getUserWalletData(string username);

#endif
