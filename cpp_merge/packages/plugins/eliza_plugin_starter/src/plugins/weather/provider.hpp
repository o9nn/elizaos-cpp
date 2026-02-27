#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_WEATHER_PROVIDER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_WEATHER_PROVIDER_H
#include "core.hpp"
// External dependency removed
#include "./types.ts.h"

extern std::shared_ptr<WeatherConfig> providerConfig;
extern std::shared_ptr<Provider> weatherProvider;
extern std::function<void(any)> initializeWeatherProvider;
#endif
