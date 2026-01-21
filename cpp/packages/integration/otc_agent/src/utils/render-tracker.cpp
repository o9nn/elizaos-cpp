#include "render-tracker.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string summarizeValue(const std::any& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (value == null) return "null";
    if (value == undefined) return "undefined";
    if (typeof value == "function") return `fn:${value.name || "anonymous"}`;
    if (typeof value == "object") {
        if (Array.isArray(value)) return `array[${value.length}]`;
        const auto keys = Object.keys(value);
        if (keys.length == 0) return "{}";
        if (keys.length <= 3) return `{${keys.join(",")}}`;
        return "{" + std::to_string(keys.slice(0, 3).join(",")) + "...+" + std::to_string(keys.size() - 3) + "}";
    }
    if (typeof value == "string") {
        return "value.size() > 20 ? " + "\"" + std::to_string(value.slice(0, 20)) + "...\"" + " : " + "\"" + value + "\"";
    }
    return std::to_string(value);

}

std::unordered_map<std::string, std::string> getPropsSnapshot(const std::unordered_map<std::string, std::any>& props) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> snapshot = {};
    for (const int [key, value] of Object.entries(props)) {
        if (key == "children") {
            snapshot[key] = typeof value == "object" ? "ReactNode" : std::to_string(value);
            } else {
                snapshot[key] = summarizeValue(value);
            }
        }
        return snapshot;

}

std::vector<std::string> findChanges(const std::unordered_map<std::string, std::string>& prev, const std::unordered_map<std::string, std::string>& curr) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!prev) return ["(first render)"];
    const std::vector<std::string> changes = [];

    const auto allKeys = new Set([...Object.keys(prev), ...Object.keys(curr)]);
    for (const auto& key : allKeys)
        if (prev[key] != curr[key]) {
            "changes.push_back(" + key + ": " + std::to_string(prev[key]) + " → " + std::to_string(curr[key])
        }
    }

    return changes.size() > 0 ? changes : ["(no prop changes detected)"];

}

void MyComponent(MyProps props) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    *   useRenderTracker("MyComponent", props);
    *   // ... rest of component
    *
}

void trackRender(const std::string& componentName, std::optional<std::unordered_map<std::string, std::any>> props, std::optional<std::unordered_map<std::string, std::any>> state) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Only run in development
        if (process.env.NODE_ENV != "development") return;

        // Skip ignored components
        if (CONFIG.ignoredComponents.has(componentName)) return;

        const auto now = Date.now();
        auto record = renderCounts.get(componentName);

        if (!record) {
            record = { count: 0, timestamps: [], firstRenderTime: now };
            renderCounts.set(componentName, record);
        }

        // Clean up old timestamps outside the window
        record.timestamps = record.timestamps.filter(;
        [&](ts) { return now - ts < CONFIG.timeWindowMs,; }
        );

        // Add current timestamp
        record.timestamps.push_back(now);
        record.count++;

        // Get current snapshots
        const auto propsSnapshot = props ? getPropsSnapshot(props) : std::nullopt;
        const auto stateSnapshot = state ? getPropsSnapshot(state) : std::nullopt;

        // Check for excessive renders
        const auto recentRenders = record.timestamps.size();

        // Determine if we're in initial mount grace period
        const auto timeSinceFirstRender = now - record.firstRenderTime;
        const auto isInitialMount =;
        timeSinceFirstRender < CONFIG.initialMountGracePeriodMs;
        const auto maxAllowed = isInitialMount;
        ? CONFIG.maxRendersInitialMount;
        : CONFIG.maxRenders;

        if (CONFIG.verboseLogging) {
            const auto propsChanges = propsSnapshot;
            ? findChanges(record.lastProps, propsSnapshot);
            : [];
            const auto stateChanges = stateSnapshot;
            ? findChanges(record.lastState, stateSnapshot);
            : [];
            console.log(
            "[RenderTracker] " + componentName + " render #" + record.count + " (" + recentRenders + " in " + CONFIG.timeWindowMs + "ms)"
            { propsChanges, stateChanges, isInitialMount },
            );
        }

        if (recentRenders > maxAllowed) {
            const auto propsChanges = propsSnapshot;
            ? findChanges(record.lastProps, propsSnapshot);
            : [];
            const auto stateChanges = stateSnapshot;
            ? findChanges(record.lastState, stateSnapshot);
            : [];

            const auto error = new Error(;
            "[RenderTracker] EXCESSIVE RENDERS DETECTED: " + componentName + " rendered " + recentRenders + " times in " + CONFIG.timeWindowMs + "ms.\n\n"
            "This indicates a render loop or severe performance issue.\n\n";
            "Recent prop changes: " + std::to_string(propsChanges.join(", ")) + "\n"
            "Recent state changes: " + std::to_string(stateChanges.join(", ")) + "\n\n"
            "Common causes:\n"
            "  1. Object/array created in render passed (use useMemo)\n";
            "  2. Callback created in render passed (use useCallback)\n";
            "  3. State update in useEffect without proper deps\n";
            "  4. Context value changing on every render\n";
            "  5. Missing dependency in useEffect causing infinite loop"
            );

            // Log detailed info before throwing
            std::cerr << error.message << std::endl;
            std::cerr << "Component:" << componentName << std::endl;
            std::cerr << "Props snapshot:" << propsSnapshot << std::endl;
            std::cerr << "State snapshot:" << stateSnapshot << std::endl;
            console.error(
            "Render timestamps:",
            record.timestamps.map((ts) => new Date(ts).toISOString()),
            );
            std::cerr << "Time since first render:" << timeSinceFirstRender << "ms" << std::endl;
            std::cerr << "Is initial mount period:" << isInitialMount << std::endl;

            throw;
        }

        // Update snapshots for next comparison
        record.lastProps = propsSnapshot;
        record.lastState = stateSnapshot;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void MyComponent(MyProps props) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    *   useRenderTracker("MyComponent", props);
    *   const [state, setState] = useState({});
    *   // ... rest of component
    *
}

void useRenderTracker(const std::string& componentName, std::optional<std::unordered_map<std::string, std::any>> props, std::optional<std::unordered_map<std::string, std::any>> state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    trackRender(componentName, props, state);

}

} // namespace elizaos
