#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_SERVER_SRC_API_RUNTIME_LOGGING_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_SERVER_SRC_API_RUNTIME_LOGGING_H
#include "core.hpp"
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)

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
