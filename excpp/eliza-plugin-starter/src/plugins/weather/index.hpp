#include "action.ts.hpp"
#include "evaluator.ts.hpp"
#include "provider.ts.hpp"
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
;
;
;

const weatherPlugin: Plugin = {
  name: "weather",
  description: "Weather information plugin with OpenWeatherMap integration",
  actions: [getWeatherAction],
  evaluators: [weatherEvaluator],
  providers: [weatherProvider],
};

const initializeWeather = (config: WeatherConfig): void => {
  initializeWeatherProvider(config);
};

* from "./types.ts";

} // namespace elizaos
