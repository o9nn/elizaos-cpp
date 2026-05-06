#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_UTILS_H
#include "core.h"
#include "node:path.h"
using path = _default;
#include "node:fs.h"
using fs = _default;
#include "dotenv.h"
using dotenv = _default;

string expandTildePath(string filepath);

string resolvePgliteDir(string dir = undefined, string fallbackDir = undefined);

#endif
