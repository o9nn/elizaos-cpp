#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/actions/walk_randomly.h"

double RANDOM_WALK_DEFAULT_INTERVAL = 4000;
double RANDOM_WALK_DEFAULT_MAX_DISTANCE = 30;
std::shared_ptr<Action> hyperfyWalkRandomlyAction = object{
    object::pair{std::string("name"), std::string("HYPERFY_WALK_RANDOMLY")}, 
    object::pair{std::string("similes"), array<string>{ std::string("WANDER"), std::string("PACE_AROUND"), std::string("WALK_AROUND"), std::string("MOVE_RANDOMLY") }}, 
    object::pair{std::string("description"), std::string("Makes your character wander to random points nearby; use for idle behavior or ambient movement.")}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        return AND((AND((!!service), (service->isConnected()))), (!!service->getWorld()->controls));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto _message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        auto controls = as<any>(world->controls);
        if (OR((OR((!service), (!world))), (!controls))) {
            logger->error(std::string("Hyperfy service, world, or controls not found for HYPERFY_WALK_RANDOMLY action."));
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Error: Cannot wander. Hyperfy connection/controls unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        if (OR((type_of(std::bind(&AgentControls::startRandomWalk, controls, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)) != std::string("function")), (type_of(std::bind(&AgentControls::stopRandomWalk, controls)) != std::string("function")))) {
            logger->error(std::string("AgentControls missing startRandomWalk or stopRandomWalk methods."));
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Error: Wander functionality not available in controls.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto command = OR((options["command"]), (std::string("start")));
        auto intervalMs = (options["interval"]) ? any(options["interval"] * 1000) : any(RANDOM_WALK_DEFAULT_INTERVAL);
        auto maxDistance = OR((options["distance"]), (RANDOM_WALK_DEFAULT_MAX_DISTANCE));
        if (command == std::string("stop")) {
            if (controls->getIsWalkingRandomly()) {
                controls->stopRandomWalk();
            } else {
            }
        } else {
            controls->startRandomWalk(intervalMs, maxDistance);
            std::async([=]() { callback(object{
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_WALK_RANDOMLY") }}, 
                object::pair{std::string("source"), std::string("hyperfy")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("status"), std::string("started")}, 
                    object::pair{std::string("intervalMs"), intervalMs}, 
                    object::pair{std::string("maxDistance"), maxDistance}
                }}
            }); });
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Wander around for a bit.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Starting to wander randomly... (New target every ~5.0s)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_WALK_RANDOMLY") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Just pace around here.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Starting to wander randomly... (New target every ~5.0s)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_WALK_RANDOMLY") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Stop wandering.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Stopped wandering.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_WALK_RANDOMLY") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
