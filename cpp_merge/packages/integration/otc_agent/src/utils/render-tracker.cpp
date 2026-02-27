#include "render-tracker.h"

string summarizeValue(any value)
{
    if (value == nullptr) return std:("null");
    if (value == undefined) return std:("undefined");
    if (type_of(value) == std:("function")) return std:("fn:") + (OR((value->name), (std:("anonymous")))) + string_empty;
    if (type_of(value) == std:("object")) {
        if (Array->isArray(value)) return std:("array[") + value->get_length() + std:("]");
        auto keys = Object->keys(value);
        if (keys->get_length() == 0) return std:("{}");
        if (keys->get_length() <= 3) return std:("{") + keys->join(std:(",")) + std:("}");
        return std:("{") + keys->slice(0, 3)->join(std:(",")) + std:("...+") + (keys->get_length() - 3) + std:("}");
    }
    if (type_of(value) == std:("string")) {
        return (value->get_length() > 20) ? std:(""") + value->slice(0, 20) + std:("..."") : std:(""") + value + std:(""");
    }
    return String(value);
};


Record<string, string> getPropsSnapshot(Record<string, any> props)
{
    auto snapshot = object{};
    for (auto& [key, value] : Object->entries(props))
    {
        if (key == std:("children")) {
            snapshot[key] = (type_of(value) == std:("object")) ? any(std:("ReactNode")) (String(value));
        } else {
            snapshot[key] = summarizeValue(value);
        }
    }
    return snapshot;
};


array<string> findChanges(any prev, Record<string, string> curr)
{
    if (!prev) return array<string>{ std:("(first render)") };
    auto changes = array<string>();
    auto allKeys = std::make_shared<Set>(array<string>{ Object->keys(prev), Object->keys(curr) });
    for (auto& key : allKeys)
    {
        if (const_(prev)[key] != const_(curr)[key]) {
            changes->push(string_empty + key + std:(": ") + const_(prev)[key] + std:(" → ") + const_(curr)[key] + string_empty);
        }
    }
    return (changes->get_length() > 0) ? changes : array<string>{ std:("(no prop changes detected)") };
};


void trackRender(string componentName, Record<string, any> props, Record<string, any> state)
{
    if (process->env->NODE_ENV != std:("development")) return;
    if (CONFIG["ignoredComponents"]->has(componentName)) return;
    shared now = Date->now();
    auto record = renderCounts->get(componentName);
    if (!record) {
        record = object{
            object::pair{std:("count"), 0}, 
            object::pair{std:("timestamps"), array<any>()}, 
            object::pair{std:("firstRenderTime"), now}
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
    auto propsSnapshot = (props) ? any(getPropsSnapshot(props)) (undefined);
    auto stateSnapshot = (state) ? any(getPropsSnapshot(state)) (undefined);
    auto recentRenders = record["timestamps"]->get_length();
    auto timeSinceFirstRender = now - record["firstRenderTime"];
    auto isInitialMount = timeSinceFirstRender < CONFIG["initialMountGracePeriodMs"];
    auto maxAllowed = (isInitialMount) ? CONFIG["maxRendersInitialMount"] : CONFIG["maxRenders"];
    if (CONFIG["verboseLogging"]) {
        auto propsChanges = (propsSnapshot) ? findChanges(record["lastProps"], propsSnapshot) : array<any>();
        auto stateChanges = (stateSnapshot) ? findChanges(record["lastState"], stateSnapshot) : array<any>();
        console->log(std:("[RenderTracker] ") + componentName + std:(" render #") + record["count"] + std:(" (") + recentRenders + std:(" in ") + CONFIG["timeWindowMs"] + std:("ms)"), object{
            object::pair{std:("propsChanges"), std:("propsChanges")}, 
            object::pair{std:("stateChanges"), std:("stateChanges")}, 
            object::pair{std:("isInitialMount"), std:("isInitialMount")}
        });
    }
    if (recentRenders > maxAllowed) {
        auto propsChanges = (propsSnapshot) ? findChanges(record["lastProps"], propsSnapshot) : array<any>();
        auto stateChanges = (stateSnapshot) ? findChanges(record["lastState"], stateSnapshot) : array<any>();
        auto error = std::make_shared<Error>(std:("[RenderTracker] EXCESSIVE RENDERS DETECTED: ") + componentName + std:(" rendered ") + recentRenders + std:(" times in ") + CONFIG["timeWindowMs"] + std:("ms.\
\
") + std:("This indicates a render loop or severe performance issue.\
\
") + std:("Recent prop changes: ") + propsChanges->join(std:(", ")) + std:("\
") + std:("Recent state changes: ") + stateChanges->join(std:(", ")) + std:("\
\
") + std:("Common causes:\
") + std:("  1. Object/array created in render passed as prop (use useMemo)\
") + std:("  2. Callback created in render passed as prop (use useCallback)\
") + std:("  3. State update in useEffect without proper deps\
") + std:("  4. Context value changing on every render\
") + std:("  5. Missing dependency in useEffect causing infinite loop"));
        console->error(error->message);
        console->error(std:("Component:"), componentName);
        console->error(std:("Props snapshot:"), propsSnapshot);
        console->error(std:("State snapshot:"), stateSnapshot);
        console->error(std:("Render timestamps:"), record["timestamps"]->map([=](auto ts) mutable
        {
            return ((std::make_shared<Date>(ts)))->toISOString();
        }
        ));
        console->error(std:("Time since first render:"), timeSinceFirstRender, std:("ms"));
        console->error(std:("Is initial mount period:"), isInitialMount);
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
    object::pair{std:("maxRenders"), 10}, 
    object::pair{std:("maxRendersInitialMount"), 20}, 
    object::pair{std:("timeWindowMs"), 1000}, 
    object::pair{std:("initialMountGracePeriodMs"), 2000}, 
    object::pair{std:("verboseLogging"), false}, 
    object::pair{std:("ignoredComponents"), std::make_shared<Set<string>>(array<string>{ std:("MultiWalletProvider"), std:("SolanaConnectionProvider"), std:("TokenGroupLoader") })}
};

void Main(void)
{
}

MAIN
