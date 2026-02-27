#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_REMOVE_UNFINISHED_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_REMOVE_UNFINISHED_H
#include "core.hpp"
#include "path.hpp"
#include "fs.hpp"
#include "../utils/files.h"
#include "../utils/log.h"

class TrajectoryFile;

extern std::shared_ptr<AgentLogger> logger;
class TrajectoryFile : public object, public std::enable_shared_from_this<TrajectoryFile> {
public:
    using std::enable_shared_from_this<TrajectoryFile>::shared_from_this;
    object info;
};

std::shared_ptr<Promise<void>> removeUnfinished(string baseDir, boolean dryRun = true);

#endif
