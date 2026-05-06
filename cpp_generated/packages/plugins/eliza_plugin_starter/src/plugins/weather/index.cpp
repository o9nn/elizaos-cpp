#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-plugin-starter/src/plugins/weather/index.h"

std::shared_ptr<Plugin> weatherPlugin = object{
    object::pair{std::string("name"), std::string("weather")}, 
    object::pair{std::string("description"), std::string("Weather information plugin with OpenWeatherMap integration")}, 
    object::pair{std::string("actions"), array<any>{ getWeatherAction }}, 
    object::pair{std::string("evaluators"), array<any>{ weatherEvaluator }}, 
    object::pair{std::string("providers"), array<any>{ weatherProvider }}
};
std::function<void(any)> initializeWeather = [=](auto config) mutable
{
    initializeWeatherProvider(config);
};

void Main(void)
{
}

MAIN
