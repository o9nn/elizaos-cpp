#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/actions/stop.h"

std::shared_ptr<Action> hyperfyStopMovingAction = object{
    object::pair{std:("name"), std:("HYPERFY_STOP_MOVING")}, 
    object::pair{std:("similes"), array<string>{ std:("STOP"), std:("HALT"), std:("STOP_WALKING"), std:("CANCEL_MOVEMENT"), std:("STOP_PATROLLING") }}, 
    object::pair{std:("description"), std:("Instantly stops your current walking or pathing; use to pause movement before speaking or performing another action.")}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto controls = as<any>(service->getWorld()->controls);
        return AND((AND((!!service), (service->isConnected()))), (!!controls));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto _message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto controls = as<any>(service->getWorld()->controls);
        if (!controls) {
            logger->error(std:("Hyperfy service or controls not found for HYPERFY_STOP_MOVING action."));
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Error: Cannot stop movement. Hyperfy connection/controls unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        if (type_of(std::bind(&AgentControls::stopAllActions, controls, std::placeholders::_1)) != std:("function")) {
            logger->error(std:("AgentControls missing stopAllActions method."));
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Error: Stop functionality not available in controls.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto reason = OR((options["reason"]), (std:("stop action called")));
        try
        {
            controls->stopAllActions(reason);
            std::async([=]() { callback(object{
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("actions"), array<string>{ std:("HYPERFY_STOP_MOVING") }}, 
                object::pair{std:("source"), std:("hyperfy")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("status"), std:("movement_stopped")}, 
                    object::pair{std:("reason"), reason}
                }}
            }); });
        }
        catch (const any& error)
        {
            logger->error(std:("Error during HYPERFY_STOP_MOVING:"), error);
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Error stopping movement: ") + error["message"] + string_empty}
            }); });
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Stop walking.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Stopped current movement. Reason: stop action called")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_STOP_MOVING") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Halt!")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Stopped current movement. Reason: stop action called")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_STOP_MOVING") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
