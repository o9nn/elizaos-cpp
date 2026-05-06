#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_H
#include "core.h"
#include "@elizaos/core.h"
#include "./tasks/birdeye.h"
using Birdeye = _default;
#include "./tasks/buySignal.h"
using BuySignal = _default;
#include "./tasks/sellSignal.h"
using SellSignal = _default;
#include "./tasks/twitter.h"
using Twitter = _default;
#include "./tasks/twitterParser.h"
using TwitterParser = _default;
#include "./types.h"

extern std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks;
#endif
