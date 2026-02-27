#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_WEATHER_PROVIDER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_WEATHER_PROVIDER_H
#include "core.h"
#include "@ai16z/eliza.h"
#include "./types.ts.h"

extern std::shared_ptr<WeatherConfig> providerConfig;
extern std::shared_ptr<Provider> weatherProvider;
extern std::function<void(any)> initializeWeatherProvider;
#endif
