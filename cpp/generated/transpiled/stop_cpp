#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/actions/stop.h"

std::shared_ptr<Action> hyperfyStopMovingAction = object{
    object::pair{std::string("name"), std::string("HYPERFY_STOP_MOVING")}, 
    object::pair{std::string("similes"), array<string>{ std::string("STOP"), std::string("HALT"), std::string("STOP_WALKING"), std::string("CANCEL_MOVEMENT"), std::string("STOP_PATROLLING") }}, 
    object::pair{std::string("description"), std::string("Instantly stops your current walking or pathing; use to pause movement before speaking or performing another action.")}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto controls = as<any>(service->getWorld()->controls);
        return AND((AND((!!service), (service->isConnected()))), (!!controls));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto _message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto controls = as<any>(service->getWorld()->controls);
        if (!controls) {
            logger->error(std::string("Hyperfy service or controls not found for HYPERFY_STOP_MOVING action."));
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Error: Cannot stop movement. Hyperfy connection/controls unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        if (type_of(std::bind(&AgentControls::stopAllActions, controls, std::placeholders::_1)) != std::string("function")) {
            logger->error(std::string("AgentControls missing stopAllActions method."));
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Error: Stop functionality not available in controls.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto reason = OR((options["reason"]), (std::string("stop action called")));
        try
        {
            controls->stopAllActions(reason);
            std::async([=]() { callback(object{
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_STOP_MOVING") }}, 
                object::pair{std::string("source"), std::string("hyperfy")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("status"), std::string("movement_stopped")}, 
                    object::pair{std::string("reason"), reason}
                }}
            }); });
        }
        catch (const any& error)
        {
            logger->error(std::string("Error during HYPERFY_STOP_MOVING:"), error);
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Error stopping movement: ") + error["message"] + string_empty}
            }); });
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Stop walking.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Stopped current movement. Reason: stop action called")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_STOP_MOVING") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Halt!")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Stopped current movement. Reason: stop action called")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_STOP_MOVING") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
