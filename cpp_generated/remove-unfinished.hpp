#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_REMOVE-UNFINISHED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_REMOVE-UNFINISHED_H
#include "core.h"
#include "path.h"
#include "fs.h"
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
