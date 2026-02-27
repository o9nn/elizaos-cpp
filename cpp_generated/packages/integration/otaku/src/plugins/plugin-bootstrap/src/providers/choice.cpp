#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-bootstrap/src/providers/choice.h"

std::shared_ptr<Provider> choiceProvider = object{
    object::pair{std::string("name"), std::string("CHOICE")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto _state) mutable
    {
        try
        {
            auto pendingTasks = std::async([=]() { runtime->getTasks(object{
                object::pair{std::string("roomId"), message->roomId}, 
                object::pair{std::string("tags"), array<string>{ std::string("AWAITING_CHOICE") }}
            }); });
            if (OR((!pendingTasks), (pendingTasks->length == 0))) {
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("tasks"), array<any>()}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("tasks"), std::string("No pending choices for the moment.")}
                    }}, 
                    object::pair{std::string("text"), std::string("No pending choices for the moment.")}
                };
            }
            auto tasksWithOptions = pendingTasks->filter([=](auto task) mutable
            {
                return task["metadata"]->options;
            }
            );
            if (tasksWithOptions->length == 0) {
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("tasks"), array<any>()}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("tasks"), std::string("No pending choices for the moment.")}
                    }}, 
                    object::pair{std::string("text"), std::string("No pending choices for the moment.")}
                };
            }
            shared output = std::string("# Pending Tasks\
\
");
            output += std::string("The following tasks are awaiting your selection:\
\
");
            tasksWithOptions->forEach([=](auto task, auto index) mutable
            {
                output += string_empty + (index + 1) + std::string(". **") + task["name"] + std::string("**\
");
                if (task["description"]) {
                    output += std::string("   ") + task["description"] + std::string("\
");
                }
                if (task["metadata"]["options"]) {
                    output += std::string("   Options:\
");
                    shared options = as<any>(task["metadata"]["options"]);
                    options->forEach([=](auto option) mutable
                    {
                        if (type_of(option) == std::string("string")) {
                            auto description = OR((task["metadata"]["options"]["find"]([=](auto o) mutable
                            {
                                return o["name"] == option;
                            }
                            )["description"]), (string_empty));
                            output += std::string("   - "") + option + std::string("" ") + (description) ? any(std::string("- ") + description + string_empty) : any(string_empty) + std::string("\
");
                        } else {
                            output += std::string("   - "") + option["name"] + std::string("" ") + (option["description"]) ? any(std::string("- ") + option["description"] + string_empty) : any(string_empty) + std::string("\
");
                        }
                    }
                    );
                }
                output += std::string("\
");
            }
            );
            output += std::string("To select an option, reply with the option name (e.g., 'post' or 'cancel').\
");
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("tasks"), tasksWithOptions}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("tasks"), output}
                }}, 
                object::pair{std::string("text"), output}
            };
        }
        catch (const any& error)
        {
            logger->error(object{
                object::pair{std::string("error"), std::string("error")}
            }, std::string("Error in options provider:"));
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("tasks"), array<any>()}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("tasks"), std::string("There was an error retrieving pending tasks with options.")}
                }}, 
                object::pair{std::string("text"), std::string("There was an error retrieving pending tasks with options.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
