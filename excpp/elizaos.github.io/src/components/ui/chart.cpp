#include "chart.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useChart() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto context = React.useContext(ChartContext);

        if (!context) {
            throw std::runtime_error("useChart must be used within a <ChartContainer />");
        }

        return context;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void getPayloadConfigFromPayload(ChartConfig config, unknown payload, const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof payload != "object" || payload == null) {
        return std::nullopt;
    }

    const auto payloadPayload =;
    "payload" in payload &&;
    typeof payload.payload == "object" &&;
    payload.payload != nullptr;
    ? payload.payload;
    : std::nullopt;

    std::string configLabelKey = key;

    if (
    key in payload &&;
    typeof payload[key typeof payload] == "string";
    ) {
        configLabelKey = payload[key typeof payload];
        } else if (;
        payloadPayload &&;
        key in payloadPayload &&;
        typeof payloadPayload[key typeof payloadPayload] == "string";
        ) {
            configLabelKey = payloadPayload[;
            key typeof payloadPayload;
            ];
        }

        return configLabelKey in config;
        ? config[configLabelKey];
        : config[key typeof config];

}

} // namespace elizaos
