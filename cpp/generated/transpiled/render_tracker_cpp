#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/utils/render-tracker.h"

string summarizeValue(any value)
{
    if (value == nullptr) return std::string("null");
    if (value == undefined) return std::string("undefined");
    if (type_of(value) == std::string("function")) return std::string("fn:") + (OR((value->name), (std::string("anonymous")))) + string_empty;
    if (type_of(value) == std::string("object")) {
        if (Array->isArray(value)) return std::string("array[") + value->get_length() + std::string("]");
        auto keys = Object->keys(value);
        if (keys->get_length() == 0) return std::string("{}");
        if (keys->get_length() <= 3) return std::string("{") + keys->join(std::string(",")) + std::string("}");
        return std::string("{") + keys->slice(0, 3)->join(std::string(",")) + std::string("...+") + (keys->get_length() - 3) + std::string("}");
    }
    if (type_of(value) == std::string("string")) {
        return (value->get_length() > 20) ? std::string(""") + value->slice(0, 20) + std::string("..."") : std::string(""") + value + std::string(""");
    }
    return String(value);
};


Record<string, string> getPropsSnapshot(Record<string, any> props)
{
    auto snapshot = object{};
    for (auto& [key, value] : Object->entries(props))
    {
        if (key == std::string("children")) {
            snapshot[key] = (type_of(value) == std::string("object")) ? any(std::string("ReactNode")) : any(String(value));
        } else {
            snapshot[key] = summarizeValue(value);
        }
    }
    return snapshot;
};


array<string> findChanges(any prev, Record<string, string> curr)
{
    if (!prev) return array<string>{ std::string("(first render)") };
    auto changes = array<string>();
    auto allKeys = std::make_shared<Set>(array<string>{ Object->keys(prev), Object->keys(curr) });
    for (auto& key : allKeys)
    {
        if (const_(prev)[key] != const_(curr)[key]) {
            changes->push(string_empty + key + std::string(": ") + const_(prev)[key] + std::string(" → ") + const_(curr)[key] + string_empty);
        }
    }
    return (changes->get_length() > 0) ? changes : array<string>{ std::string("(no prop changes detected)") };
};


void trackRender(string componentName, Record<string, any> props, Record<string, any> state)
{
    if (process->env->NODE_ENV != std::string("development")) return;
    if (CONFIG["ignoredComponents"]->has(componentName)) return;
    shared now = Date->now();
    auto record = renderCounts->get(componentName);
    if (!record) {
        record = object{
            object::pair{std::string("count"), 0}, 
            object::pair{std::string("timestamps"), array<any>()}, 
            object::pair{std::string("firstRenderTime"), now}
        };
        renderCounts->set(componentName, record);
    }
    record["timestamps"] = record["timestamps"]->filter([=](auto ts) mutable
    {
        return now - ts < CONFIG["timeWindowMs"];
    }
    );
    record["timestamps"]->push(now);
    record["count"]++;
    auto propsSnapshot = (props) ? any(getPropsSnapshot(props)) : any(undefined);
    auto stateSnapshot = (state) ? any(getPropsSnapshot(state)) : any(undefined);
    auto recentRenders = record["timestamps"]->get_length();
    auto timeSinceFirstRender = now - record["firstRenderTime"];
    auto isInitialMount = timeSinceFirstRender < CONFIG["initialMountGracePeriodMs"];
    auto maxAllowed = (isInitialMount) ? CONFIG["maxRendersInitialMount"] : CONFIG["maxRenders"];
    if (CONFIG["verboseLogging"]) {
        auto propsChanges = (propsSnapshot) ? findChanges(record["lastProps"], propsSnapshot) : array<any>();
        auto stateChanges = (stateSnapshot) ? findChanges(record["lastState"], stateSnapshot) : array<any>();
        console->log(std::string("[RenderTracker] ") + componentName + std::string(" render #") + record["count"] + std::string(" (") + recentRenders + std::string(" in ") + CONFIG["timeWindowMs"] + std::string("ms)"), object{
            object::pair{std::string("propsChanges"), std::string("propsChanges")}, 
            object::pair{std::string("stateChanges"), std::string("stateChanges")}, 
            object::pair{std::string("isInitialMount"), std::string("isInitialMount")}
        });
    }
    if (recentRenders > maxAllowed) {
        auto propsChanges = (propsSnapshot) ? findChanges(record["lastProps"], propsSnapshot) : array<any>();
        auto stateChanges = (stateSnapshot) ? findChanges(record["lastState"], stateSnapshot) : array<any>();
        auto error = std::make_shared<Error>(std::string("[RenderTracker] EXCESSIVE RENDERS DETECTED: ") + componentName + std::string(" rendered ") + recentRenders + std::string(" times in ") + CONFIG["timeWindowMs"] + std::string("ms.\
\
") + std::string("This indicates a render loop or severe performance issue.\
\
") + std::string("Recent prop changes: ") + propsChanges->join(std::string(", ")) + std::string("\
") + std::string("Recent state changes: ") + stateChanges->join(std::string(", ")) + std::string("\
\
") + std::string("Common causes:\
") + std::string("  1. Object/array created in render passed as prop (use useMemo)\
") + std::string("  2. Callback created in render passed as prop (use useCallback)\
") + std::string("  3. State update in useEffect without proper deps\
") + std::string("  4. Context value changing on every render\
") + std::string("  5. Missing dependency in useEffect causing infinite loop"));
        console->error(error->message);
        console->error(std::string("Component:"), componentName);
        console->error(std::string("Props snapshot:"), propsSnapshot);
        console->error(std::string("State snapshot:"), stateSnapshot);
        console->error(std::string("Render timestamps:"), record["timestamps"]->map([=](auto ts) mutable
        {
            return ((std::make_shared<Date>(ts)))->toISOString();
        }
        ));
        console->error(std::string("Time since first render:"), timeSinceFirstRender, std::string("ms"));
        console->error(std::string("Is initial mount period:"), isInitialMount);
        throw any(error);
    }
    record["lastProps"] = propsSnapshot;
    record["lastState"] = stateSnapshot;
};


void useRenderTracker(string componentName, Record<string, any> props, Record<string, any> state)
{
    trackRender(componentName, props, state);
};


std::shared_ptr<Map<string, std::shared_ptr<RenderRecord>>> renderCounts = std::make_shared<Map<string, RenderRecord>>();
object CONFIG = object{
    object::pair{std::string("maxRenders"), 10}, 
    object::pair{std::string("maxRendersInitialMount"), 20}, 
    object::pair{std::string("timeWindowMs"), 1000}, 
    object::pair{std::string("initialMountGracePeriodMs"), 2000}, 
    object::pair{std::string("verboseLogging"), false}, 
    object::pair{std::string("ignoredComponents"), std::make_shared<Set<string>>(array<string>{ std::string("MultiWalletProvider"), std::string("SolanaConnectionProvider"), std::string("TokenGroupLoader") })}
};

void Main(void)
{
}

MAIN
