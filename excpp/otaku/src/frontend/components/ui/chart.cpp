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

void ChartContainer(auto {
  id, auto className, auto children, auto config, ComponentProps<"div"> & {
  config: ChartConfig;
  children: ComponentProps<
    typeof RechartsPrimitive.ResponsiveContainer
  >["children"];
} ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto uniqueId = React.useId();
    const auto chartId = "chart-" + std::to_string(id || uniqueId.replace(/:/g, ""));

    return (;
    <ChartContext.Provider value={{ config }}>;
    <div;
    data-slot="chart";
    data-chart={chartId}
    className={cn(;
    "[&_.recharts-cartesian-axis-tick_text]:fill-muted-foreground [&_.recharts-cartesian-grid_line[stroke='#ccc']]:stroke-border/50 [&_.recharts-curve.recharts-tooltip-cursor]:stroke-border [&_.recharts-polar-grid_[stroke='#ccc']]:stroke-border [&_.recharts-radial-bar-background-sector]:fill-muted [&_.recharts-rectangle.recharts-tooltip-cursor]:fill-muted [&_.recharts-reference-line_[stroke='#ccc']]:stroke-border flex aspect-video justify-center text-xs [&_.recharts-dot[stroke='#fff']]:stroke-transparent [&_.recharts-layer]:outline-hidden [&_.recharts-sector]:outline-hidden [&_.recharts-sector[stroke='#fff']]:stroke-transparent [&_.recharts-surface]:outline-hidden",
    className;
    )}
    {...props}
    >;
    <ChartStyle id={chartId} config={config} />;
    <RechartsPrimitive.ResponsiveContainer>;
    {children}
    </RechartsPrimitive.ResponsiveContainer>;
    </div>;
    </ChartContext.Provider>;
    );

}

void ChartLegendContent(auto {
  className, auto hideIcon = false, auto payload, auto verticalAlign = "bottom", auto nameKey, std::optional<ComponentProps<"div"> & {
  payload: Array<{
    value: string;
    dataKey: string;
    color: string;
    [key: string]: unknown;
  }>;
  verticalAlign: "top" | "bottom";
  hideIcon: boolean;
  nameKey: string;
}> }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { config } = useChart();

    if (!payload.length) {
        return nullptr;
    }

    return (;
    <div;
    className={cn(;
    "flex items-center justify-center gap-4",
    verticalAlign == "top" ? "pb-3" : "pt-3",
    className;
    )}
    >;
    {payload.map((item) => {
        const auto key = std::to_string(nameKey || item.dataKey || "value");
        const auto itemConfig = getPayloadConfigFromPayload(config, item, key);

        return (;
        <div;
    key={item.value}
    className={cn(;
    "[&>svg]:text-muted-foreground flex items-center gap-1.5 [&>svg]:h-3 [&>svg]:w-3"
    )}
    >;
    {itemConfig.icon && !hideIcon ? (;
    <itemConfig.icon />;
    ) : (
    <div;
    className="h-2 w-2 shrink-0 rounded-[2px]";
    style={{
        backgroundColor: item.color,
    }}
    />;
    )}
    {itemConfig.label}
    </div>;
    );
    })}
    </div>;
    );

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
