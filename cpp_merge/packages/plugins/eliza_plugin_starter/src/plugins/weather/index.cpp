#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-plugin-starter/src/plugins/weather/index.h"

std::shared_ptr<Plugin> weatherPlugin = object{
    object::pair{std:("name"), std:("weather")}, 
    object::pair{std:("description"), std:("Weather information plugin with OpenWeatherMap integration")}, 
    object::pair{std:("actions"), array<any>{ getWeatherAction }}, 
    object::pair{std:("evaluators"), array<any>{ weatherEvaluator }}, 
    object::pair{std:("providers"), array<any>{ weatherProvider }}
};
std::function<void(any)> initializeWeather = [=](auto config) mutable
{
    initializeWeatherProvider(config);
};

void Main(void)
{
}

MAIN
