#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_LOGGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_LOGGER_H
#include "core.hpp"
// External dependency removed
using coreLogger = logger;

typedef std::function<void(array<any>)> LogMethod;


extern Record<any, LogMethod> logger;
extern std::shared_ptr<Record<any, std::shared_ptr<LogMethod>>> elizaLogger;
#endif
