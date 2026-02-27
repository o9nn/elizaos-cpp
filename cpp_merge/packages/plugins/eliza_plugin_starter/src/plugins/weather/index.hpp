#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_WEATHER_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_WEATHER_INDEX_H
#include "core.hpp"
// External dependency removed
#include "./action.ts.h"
#include "./evaluator.ts.h"
#include "./provider.ts.h"
#include "./types.ts.h"

extern std::shared_ptr<Plugin> weatherPlugin;
extern std::function<void(any)> initializeWeather;
#endif
