#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Format: { THEME_NAME: CSS_SELECTOR }

using ChartConfig = {

using ChartContextProps = {

void useChart();

void ChartContainer(auto className, auto children, auto config);

void ChartLegendContent(auto hideIcon, auto payload, auto verticalAlign, auto nameKey);

// Helper to extract item config from a payload.
void getPayloadConfigFromPayload(ChartConfig config, unknown payload, const std::string& key);


} // namespace elizaos
