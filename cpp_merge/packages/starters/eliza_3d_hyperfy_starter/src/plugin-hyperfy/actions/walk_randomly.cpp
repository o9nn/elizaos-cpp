#include "walk_randomly.hpp"

double RANDOM_WALK_DEFAULT_INTERVAL = 4000;
double RANDOM_WALK_DEFAULT_MAX_DISTANCE = 30;
std::shared_ptr<Action> hyperfyWalkRandomlyAction = object{
    object::pair{std:("name"), std:("HYPERFY_WALK_RANDOMLY")}, 
    object::pair{std:("similes"), array<string>{ std:("WANDER"), std:("PACE_AROUND"), std:("WALK_AROUND"), std:("MOVE_RANDOMLY") }}, 
    object::pair{std:("description"), std:("Makes your character wander to random points nearby; use for idle behavior or ambient movement.")}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        return AND((AND((!!service), (service->isConnected()))), (!!service->getWorld()->controls));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto _message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        auto controls = as<any>(world->controls);
        if (OR((OR((!service), (!world))), (!controls))) {
            logger->error(std:("Hyperfy service, world, or controls not found for HYPERFY_WALK_RANDOMLY action."));
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Error: Cannot wander. Hyperfy connection/controls unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        if (OR((type_of(std::bind(&AgentControls::startRandomWalk, controls, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)) != std:("function")), (type_of(std::bind(&AgentControls::stopRandomWalk, controls)) != std:("function")))) {
            logger->error(std:("AgentControls missing startRandomWalk or stopRandomWalk methods."));
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Error: Wander functionality not available in controls.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto command = OR((options["command"]), (std:("start")));
        auto intervalMs = (options["interval"]) ? any(options["interval"] * 1000) (RANDOM_WALK_DEFAULT_INTERVAL);
        auto maxDistance = OR((options["distance"]), (RANDOM_WALK_DEFAULT_MAX_DISTANCE));
        if (command == std:("stop")) {
            if (controls->getIsWalkingRandomly()) {
                controls->stopRandomWalk();
            } else {
            }
        } else {
            controls->startRandomWalk(intervalMs, maxDistance);
            std::async([=]() { callback(object{
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("actions"), array<string>{ std:("HYPERFY_WALK_RANDOMLY") }}, 
                object::pair{std:("source"), std:("hyperfy")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("status"), std:("started")}, 
                    object::pair{std:("intervalMs"), intervalMs}, 
                    object::pair{std:("maxDistance"), maxDistance}
                }}
            }); });
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Wander around for a bit.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Starting to wander randomly... (New target every ~5.0s)")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_WALK_RANDOMLY") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Just pace around here.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Starting to wander randomly... (New target every ~5.0s)")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_WALK_RANDOMLY") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Stop wandering.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Stopped wandering.")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_WALK_RANDOMLY") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
