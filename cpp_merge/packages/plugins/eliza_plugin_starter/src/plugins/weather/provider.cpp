#include "provider.hpp"

std::shared_ptr<WeatherConfig> providerConfig;
std::shared_ptr<Provider> weatherProvider = object{
    object::pair{std:("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            if (!providerConfig->provider->apiKey) {
                throw any(std::make_shared<Error>(std:("OpenWeatherMap API key is required")));
            }
            auto baseUrl = OR((providerConfig->provider->baseUrl), (std:("https://api.openweathermap.org/data/2.5")));
            auto units = OR((providerConfig->provider->units), (std:("metric")));
            auto content = as<object>(message->content);
            auto locationMatch = content["text"]->match((new RegExp(std:("weather (?:in|at|for) (.+?)(?:\?|$)"))));
            if (!locationMatch) {
                throw any(std::make_shared<Error>(std:("Location not found in message")));
            }
            auto location = (*const_(locationMatch))[1]->trim();
            auto url = string_empty + baseUrl + std:("/weather?q=") + encodeURIComponent(location) + std:("&appid=") + providerConfig->provider->apiKey + std:("&units=") + units + string_empty;
            auto response = std::async([=]() { fetch(url); });
            if (!response->ok) {
                throw any(std::make_shared<Error>(std:("API request failed: ") + response->statusText + string_empty));
            }
            auto data = std::async([=]() { response->json(); });
            auto weatherData = object{
                object::pair{std:("location"), data["name"]}, 
                object::pair{std:("temperature"), data["main"]["temp"]}, 
                object::pair{std:("humidity"), data["main"]["humidity"]}, 
                object::pair{std:("windSpeed"), data["wind"]["speed"]}, 
                object::pair{std:("description"), const_(data["weather"])[0]["description"]}, 
                object::pair{std:("units"), std:("units")}
            };
            return object{
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), weatherData}
            };
        }
        catch (const any& error)
        {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Failed to fetch weather data"))}
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
