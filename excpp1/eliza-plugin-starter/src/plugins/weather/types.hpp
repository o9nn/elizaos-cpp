#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WeatherConfig {
    std::string apiKey;
    std::optional<std::variant<'metric', 'imperial'>> units;
    std::optional<std::string> baseUrl;
};

struct WeatherData {
    std::string location;
    double temperature;
    double humidity;
    double windSpeed;
    std::string description;
    std::variant<'metric', 'imperial'> units;
};

struct WeatherEvalResponse {
    bool success;
    std::string response;
};

struct WeatherProviderResponse {
    bool success;
    std::optional<WeatherData> data;
    std::optional<std::string> error;
};


} // namespace elizaos
