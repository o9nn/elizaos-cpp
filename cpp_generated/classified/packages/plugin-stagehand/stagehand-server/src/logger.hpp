#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_LOGGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_LOGGER_H
#include "core.h"

class Logger;

class Logger : public object, public std::enable_shared_from_this<Logger> {
public:
    using std::enable_shared_from_this<Logger>::shared_from_this;
    string prefix = std::string("[StagehandServer]");

    template <typename /* spread: Args */>
    void info(string message, Args... args_);
    template <typename /* spread: Args */>
    void debug(string message, Args... args_);
    virtual void error(string message, any error = undefined);
    template <typename /* spread: Args */>
    void warn(string message, Args... args_);
};

template <typename /* spread: Args */>
void Logger::info(string message, Args... args_)
{
    array<any> args = array<any>{args_...};
    console->log(string_empty + this->prefix + std::string(" INFO:"), message, const_(args)[0], const_(args)[1]);
}

template <typename /* spread: Args */>
void Logger::debug(string message, Args... args_)
{
    array<any> args = array<any>{args_...};
    if (process->env->DEBUG) {
        console->log(string_empty + this->prefix + std::string(" DEBUG:"), message, const_(args)[0], const_(args)[1]);
    }
}

template <typename /* spread: Args */>
void Logger::warn(string message, Args... args_)
{
    array<any> args = array<any>{args_...};
    console->warn(string_empty + this->prefix + std::string(" WARN:"), message, const_(args)[0], const_(args)[1]);
}

#endif
