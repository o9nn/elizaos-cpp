#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_RUN-TRAJ-TO-DEMO_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_RUN-TRAJ-TO-DEMO_H
#include "core.h"
#include "path.h"
#include "fs.h"
#include "js-yaml.h"
#include "../utils/log.h"

extern std::shared_ptr<AgentLogger> logger;
extern string DEMO_COMMENT;
void saveDemo(any data, string file, string trajPath);

void convertTrajToActionDemo(string trajPath, string outputFile, boolean includeUser = false);

void trajToDemo(string trajPath, string outputDir = std::string("./demos"), string suffix = string_empty, boolean overwrite = false, boolean includeUser = false);

#endif
