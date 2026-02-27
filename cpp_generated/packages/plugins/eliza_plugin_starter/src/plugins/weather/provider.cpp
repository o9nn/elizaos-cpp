#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-plugin-starter/src/plugins/weather/provider.h"

std::shared_ptr<WeatherConfig> providerConfig;
std::shared_ptr<Provider> weatherProvider = object{
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            if (!providerConfig->provider->apiKey) {
                throw any(std::make_shared<Error>(std::string("OpenWeatherMap API key is required")));
            }
            auto baseUrl = OR((providerConfig->provider->baseUrl), (std::string("https://api.openweathermap.org/data/2.5")));
            auto units = OR((providerConfig->provider->units), (std::string("metric")));
            auto content = as<object>(message->content);
            auto locationMatch = content["text"]->match((new RegExp(std::string("weather (?:in|at|for) (.+?)(?:\?|$)"))));
            if (!locationMatch) {
                throw any(std::make_shared<Error>(std::string("Location not found in message")));
            }
            auto location = (*const_(locationMatch))[1]->trim();
            auto url = string_empty + baseUrl + std::string("/weather?q=") + encodeURIComponent(location) + std::string("&appid=") + providerConfig->provider->apiKey + std::string("&units=") + units + string_empty;
            auto response = std::async([=]() { fetch(url); });
            if (!response->ok) {
                throw any(std::make_shared<Error>(std::string("API request failed: ") + response->statusText + string_empty));
            }
            auto data = std::async([=]() { response->json(); });
            auto weatherData = object{
                object::pair{std::string("location"), data["name"]}, 
                object::pair{std::string("temperature"), data["main"]["temp"]}, 
                object::pair{std::string("humidity"), data["main"]["humidity"]}, 
                object::pair{std::string("windSpeed"), data["wind"]["speed"]}, 
                object::pair{std::string("description"), const_(data["weather"])[0]["description"]}, 
                object::pair{std::string("units"), std::string("units")}
            };
            return object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), weatherData}
            };
        }
        catch (const any& error)
        {
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Failed to fetch weather data"))}
            };
        }
    }
    }
};
std::function<void(any)> initializeWeatherProvider = [=](auto config) mutable
{
    providerConfig = config;
};

void Main(void)
{
}

MAIN
