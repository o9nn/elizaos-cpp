#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_LOGGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_LOGGER_H
#include "core.h"
#include "@elizaos/core.h"
using coreLogger = logger;

typedef std::function<void(array<any>)> LogMethod;


extern Record<any, LogMethod> logger;
extern std::shared_ptr<Record<any, std::shared_ptr<LogMethod>>> elizaLogger;
#endif
