#include "types.ts.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;

let providerConfig: WeatherConfig;

const weatherProvider: Provider = {
  get: async (
    runtime: IAgentRuntime,
    message: Memory,
    state?: State,
  ): Promise<WeatherProviderResponse> => {
    try {
      if (!providerConfig?.provider?.apiKey) {
        throw new Error("OpenWeatherMap API key is required");
      }

      const baseUrl =
        providerConfig.provider.baseUrl ||
        "https://api.openweathermap.org/data/2.5";
      const units = providerConfig.provider.units || "metric";

      // Extract location from message content
      const content = message.content as { text: string };
      const locationMatch = content.text.match(
        /weather (?:in|at|for) (.+?)(?:\?|$)/i,
      );
      if (!locationMatch) {
        throw new Error("Location not found in message");
      }
      const location = locationMatch[1].trim();

      // Format API URL
      const url = `${baseUrl}/weather?q=${encodeURIComponent(location)}&appid=${providerConfig.provider.apiKey}&units=${units}`;

      // Fetch weather data
      const response = await fetch(url);
      if (!response.ok) {
        throw new Error(`API request failed: ${response.statusText}`);
      }

      const data = await response.json();

      // Transform API response to WeatherData
      const weatherData: WeatherData = {
        location: data.name,
        temperature: data.main.temp,
        humidity: data.main.humidity,
        windSpeed: data.wind.speed,
        description: data.weather[0].description,
        units,
      };

      return {
        success: true,
        data: weatherData,
      };
    } catch (error) {
      return {
        success: false,
        error:
          error instanceof Error
            ? error.message
            : "Failed to fetch weather data",
      };
    }
  },
};

const initializeWeatherProvider = (config: WeatherConfig): void => {
  providerConfig = config;
};

} // namespace elizaos
