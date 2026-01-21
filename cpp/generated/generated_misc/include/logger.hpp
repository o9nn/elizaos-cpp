#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TEST_HYBRID_SRC_LOGGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TEST_HYBRID_SRC_LOGGER_H
#include "core.h"

class Logger;

class Logger : public object, public std::enable_shared_from_this<Logger> {
public:
    using std::enable_shared_from_this<Logger>::shared_from_this;
    std::string prefix;

    Logger(std::string prefix = std::string("ElizaOS"));
    virtual void log(std::string level, std::string message);
    virtual void info(std::string message);
    virtual void warn(std::string message);
    virtual void error(std::string message);
    virtual void debug(std::string message);
};

std::shared_ptr<Logger> createLogger(std::string prefix = undefined);

#endif
