#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_LOGGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_LOGGER_H
#include "core.h"
#include "../../logger.h"
using coreLogger = logger;

typedef std::function<void(array<any>)> LogMethod;


extern Record<any, LogMethod> logger;
extern std::shared_ptr<Record<any, std::shared_ptr<LogMethod>>> elizaLogger;
#endif
