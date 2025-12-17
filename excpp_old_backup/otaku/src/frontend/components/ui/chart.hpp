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



// Format: { THEME_NAME: CSS_SELECTOR }

using ChartConfig = {

using ChartContextProps = {

void useChart();

// Helper to extract item config from a payload.
void getPayloadConfigFromPayload(ChartConfig config, unknown payload, const std::string& key);


} // namespace elizaos
