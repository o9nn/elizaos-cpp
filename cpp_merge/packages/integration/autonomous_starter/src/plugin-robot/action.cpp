#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/action.h"

std::shared_ptr<Action> performScreenAction = object{
    object::pair{std:("name"), std:("PERFORM_SCREEN_ACTION")}, 
    object::pair{std:("similes"), array<string>{ std:("SCREEN_ACTION"), std:("CONTROL_SCREEN"), std:("INTERACT_SCREEN") }}, 
    object::pair{std:("description"), std:("Perform mouse and keyboard actions on the host screen. Options should include a list of steps with actions like move, click, and type.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
    {
        auto service = runtime->getService<std::shared_ptr<RobotService>>(as<any>(std:("ROBOT")));
        return !!service;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto _message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<RobotService>>(as<any>(std:("ROBOT")));
        if (!service) {
            logger->warn(std:("[performScreenAction] RobotService not available"));
            std::async([=]() { callback(object{
                object::pair{std:("thought"), std:("RobotService not available")}, 
                object::pair{std:("text"), std:("Unable to perform screen action - robot service is not available.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        if (OR((OR((!options["steps"]), (!Array->isArray(options["steps"])))), (options["steps"]->get_length() == 0))) {
            logger->warn(std:("[performScreenAction] No valid steps provided"));
            std::async([=]() { callback(object{
                object::pair{std:("thought"), std:("No valid steps provided")}, 
                object::pair{std:("text"), std:("Unable to perform screen action - no valid steps were provided.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto executedActions = 0;
        auto actionSummary = array<string>();
        try
        {
            for (auto& step : options["steps"])
            {
                if (AND((AND((step["action"] == std:("move")), (step["x"] != undefined))), (step["y"] != undefined))) {
                    service->moveMouse(step["x"], step["y"]);
                    actionSummary->push(std:("moved mouse to (") + step["x"] + std:(", ") + step["y"] + std:(")"));
                    executedActions++;
                } else if (step["action"] == std:("click")) {
                    auto button = OR((step["button"]), (std:("left")));
                    service->click(button, false);
                    actionSummary->push(std:("clicked ") + button + std:(" mouse button"));
                    executedActions++;
                } else if (AND((step["action"] == std:("type")), (step["text"]))) {
                    service->typeText(step["text"]);
                    actionSummary->push(std:("typed "") + step["text"] + std:("""));
                    executedActions++;
                } else {
                    logger->warn(std:("[performScreenAction] Invalid step:"), step);
                    actionSummary->push(std:("skipped invalid step: ") + JSON->stringify(step) + string_empty);
                }
            }
            logger->info(std:("[performScreenAction] Executed ") + executedActions + std:(" actions: ") + actionSummary->join(std:(", ")) + string_empty);
            std::async([=]() { callback(object{
                object::pair{std:("thought"), std:("Executed ") + executedActions + std:(" screen actions successfully")}, 
                object::pair{std:("text"), std:("Screen actions completed: ") + actionSummary->join(std:(", ")) + std:(".")}
            }); });
        }
        catch (const any& error)
        {
            logger->error(std:("[performScreenAction] Error executing screen actions:"), error);
            std::async([=]() { callback(object{
                object::pair{std:("thought"), std:("Error occurred while executing screen actions")}, 
                object::pair{std:("text"), std:("Failed to complete screen actions. Executed ") + executedActions + std:(" actions before error occurred.")}
            }); });
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("click the submit button")}
        }}
    }, object{
        object::pair{std:("name"), std:("agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("PERFORM_SCREEN_ACTION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("fill in the email field with test@example.com")}
        }}
    }, object{
        object::pair{std:("name"), std:("agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("PERFORM_SCREEN_ACTION") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
