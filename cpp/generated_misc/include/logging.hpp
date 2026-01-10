#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_RUNTIME_LOGGING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_RUNTIME_LOGGING_H
#include "core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;

typedef any LogLevel;

class LogEntry;

extern object LOG_LEVELS;
class LogEntry : public object, public std::enable_shared_from_this<LogEntry> {
public:
    using std::enable_shared_from_this<LogEntry>::shared_from_this;
    double level;

    double time;

    string msg;
};

std::shared_ptr<express::Router> createLoggingRouter();

#endif
