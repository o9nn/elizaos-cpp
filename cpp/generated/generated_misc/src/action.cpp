#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/action.h"

std::shared_ptr<Action> performScreenAction = object{
    object::pair{std::string("name"), std::string("PERFORM_SCREEN_ACTION")}, 
    object::pair{std::string("similes"), array<string>{ std::string("SCREEN_ACTION"), std::string("CONTROL_SCREEN"), std::string("INTERACT_SCREEN") }}, 
    object::pair{std::string("description"), std::string("Perform mouse and keyboard actions on the host screen. Options should include a list of steps with actions like move, click, and type.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
    {
        auto service = runtime->getService<std::shared_ptr<RobotService>>(as<any>(std::string("ROBOT")));
        return !!service;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto _message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<RobotService>>(as<any>(std::string("ROBOT")));
        if (!service) {
            logger->warn(std::string("[performScreenAction] RobotService not available"));
            std::async([=]() { callback(object{
                object::pair{std::string("thought"), std::string("RobotService not available")}, 
                object::pair{std::string("text"), std::string("Unable to perform screen action - robot service is not available.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        if (OR((OR((!options["steps"]), (!Array->isArray(options["steps"])))), (options["steps"]->get_length() == 0))) {
            logger->warn(std::string("[performScreenAction] No valid steps provided"));
            std::async([=]() { callback(object{
                object::pair{std::string("thought"), std::string("No valid steps provided")}, 
                object::pair{std::string("text"), std::string("Unable to perform screen action - no valid steps were provided.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto executedActions = 0;
        auto actionSummary = array<string>();
        try
        {
            for (auto& step : options["steps"])
            {
                if (AND((AND((step["action"] == std::string("move")), (step["x"] != undefined))), (step["y"] != undefined))) {
                    service->moveMouse(step["x"], step["y"]);
                    actionSummary->push(std::string("moved mouse to (") + step["x"] + std::string(", ") + step["y"] + std::string(")"));
                    executedActions++;
                } else if (step["action"] == std::string("click")) {
                    auto button = OR((step["button"]), (std::string("left")));
                    service->click(button, false);
                    actionSummary->push(std::string("clicked ") + button + std::string(" mouse button"));
                    executedActions++;
                } else if (AND((step["action"] == std::string("type")), (step["text"]))) {
                    service->typeText(step["text"]);
                    actionSummary->push(std::string("typed "") + step["text"] + std::string("""));
                    executedActions++;
                } else {
                    logger->warn(std::string("[performScreenAction] Invalid step:"), step);
                    actionSummary->push(std::string("skipped invalid step: ") + JSON->stringify(step) + string_empty);
                }
            }
            logger->info(std::string("[performScreenAction] Executed ") + executedActions + std::string(" actions: ") + actionSummary->join(std::string(", ")) + string_empty);
            std::async([=]() { callback(object{
                object::pair{std::string("thought"), std::string("Executed ") + executedActions + std::string(" screen actions successfully")}, 
                object::pair{std::string("text"), std::string("Screen actions completed: ") + actionSummary->join(std::string(", ")) + std::string(".")}
            }); });
        }
        catch (const any& error)
        {
            logger->error(std::string("[performScreenAction] Error executing screen actions:"), error);
            std::async([=]() { callback(object{
                object::pair{std::string("thought"), std::string("Error occurred while executing screen actions")}, 
                object::pair{std::string("text"), std::string("Failed to complete screen actions. Executed ") + executedActions + std::string(" actions before error occurred.")}
            }); });
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("click the submit button")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("actions"), array<string>{ std::string("PERFORM_SCREEN_ACTION") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("fill in the email field with test@example.com")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("agent")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("actions"), array<string>{ std::string("PERFORM_SCREEN_ACTION") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
