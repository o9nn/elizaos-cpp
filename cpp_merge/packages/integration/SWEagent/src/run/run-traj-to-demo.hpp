#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_RUN_TRAJ_TO_DEMO_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_RUN_TRAJ_TO_DEMO_H
#include "core.hpp"
#include <string>
#include "path.hpp"
#include "fs.hpp"
#include "js-yaml.h"
#include "../utils/log.h"

extern std::shared_ptr<AgentLogger> logger;
extern string DEMO_COMMENT;
void saveDemo(any data, string file, string trajPath);

void convertTrajToActionDemo(string trajPath, string outputFile, boolean includeUser = false);

void trajToDemo(string trajPath, string outputDir = std::string("./demos"), string suffix = string_empty, boolean overwrite = false, boolean includeUser = false);

#endif
