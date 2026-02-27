#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUN_TASKS_TWITTERPARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUN_TASKS_TWITTERPARSER_H
#include "core.hpp"
// External dependency removed
#include "../types.h"

class TwitterParser;

extern std::function<string(array<string>)> makeBulletpointList;
extern array<string> examples;
extern string rolePrompt;
extern string template;
class TwitterParser : public object, public std::enable_shared_from_this<TwitterParser> {
public:
    using std::enable_shared_from_this<TwitterParser>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::shared_ptr<UUID> roomId;

    TwitterParser(std::shared_ptr<IAgentRuntime> runtime);
    virtual void fillTimeframe();
    virtual any parseTweets();
};

using _default = TwitterParser;
#endif
