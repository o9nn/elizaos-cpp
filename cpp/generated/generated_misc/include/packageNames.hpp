#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGINS-AUTOMATION_SRC_PACKAGENAMES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGINS-AUTOMATION_SRC_PACKAGENAMES_H
#include "core.h"
#include "@octokit/rest.h"
#include "process.h"
using process = _default;
#include "dotenv.h"
using dotenv = _default;

extern string ORG_NAME;
extern string TARGET_BRANCH;
std::shared_ptr<Promise<void>> main();

#endif
