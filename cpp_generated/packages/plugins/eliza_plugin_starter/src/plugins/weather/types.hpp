#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_WEATHER_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-PLUGIN-STARTER_SRC_PLUGINS_WEATHER_TYPES_H
#include "core.h"
#include "@ai16z/eliza.h"

class WeatherConfig;
class WeatherData;
class WeatherActionContent;
class WeatherEvalContent;
class WeatherEvalResponse;
class WeatherProviderResponse;

class WeatherConfig : public object, public std::enable_shared_from_this<WeatherConfig> {
public:
    using std::enable_shared_from_this<WeatherConfig>::shared_from_this;
    object provider;
};

class WeatherData : public object, public std::enable_shared_from_this<WeatherData> {
public:
    using std::enable_shared_from_this<WeatherData>::shared_from_this;
    string location;

    double temperature;

    double humidity;

    double windSpeed;

    string description;

    any units;
};

class WeatherActionContent : public Content, public std::enable_shared_from_this<WeatherActionContent> {
public:
    using std::enable_shared_from_this<WeatherActionContent>::shared_from_this;
    string text;
};

class WeatherEvalContent : public Content, public std::enable_shared_from_this<WeatherEvalContent> {
public:
    using std::enable_shared_from_this<WeatherEvalContent>::shared_from_this;
    string text;
};

class WeatherEvalResponse : public object, public std::enable_shared_from_this<WeatherEvalResponse> {
public:
    using std::enable_shared_from_this<WeatherEvalResponse>::shared_from_this;
    boolean success;

    string response;
};

class WeatherProviderResponse : public object, public std::enable_shared_from_this<WeatherProviderResponse> {
public:
    using std::enable_shared_from_this<WeatherProviderResponse>::shared_from_this;
    boolean success;

    std::shared_ptr<WeatherData> data;

    string error;
};

#endif
