#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_TEST_HYBRID_SRC_LOGGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_TEST_HYBRID_SRC_LOGGER_H
#include "core.hpp"

class Logger;

class Logger : public object, public std::enable_shared_from_this<Logger> {
public:
    using std::enable_shared_from_this<Logger>::shared_from_this;
    string prefix;

    Logger(string prefix = std:("ElizaOS"));
    virtual void log(string level, string message);
    virtual void info(string message);
    virtual void warn(string message);
    virtual void error(string message);
    virtual void debug(string message);
};

std::shared_ptr<Logger> createLogger(string prefix = undefined);

#endif
