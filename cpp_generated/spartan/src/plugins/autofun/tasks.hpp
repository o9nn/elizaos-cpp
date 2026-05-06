#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTOFUN_TASKS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTOFUN_TASKS_H
#include "core.h"
#include "@elizaos/core.h"
#include "./tasks/chat.h"
using Chat = _default;
#include "./tasks/twitter.h"
using Twitter = _default;
#include "./tasks/twitterParser.h"
using TwitterParser = _default;

extern std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks;
#endif
