#include "choice.hpp"

std::shared_ptr<Provider> choiceProvider = object{
    object::pair{std:("name"), std:("CHOICE")}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto _state) mutable
    {
        try
        {
            auto pendingTasks = std::async([=]() { runtime->getTasks(object{
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("AWAITING_CHOICE") }}
            }); });
            if (OR((!pendingTasks), (pendingTasks->length == 0))) {
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("tasks"), array<any>()}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("tasks"), std:("No pending choices for the moment.")}
                    }}, 
                    object::pair{std:("text"), std:("No pending choices for the moment.")}
                };
            }
            auto tasksWithOptions = pendingTasks->filter([=](auto task) mutable
            {
                return task["metadata"]->options;
            }
            );
            if (tasksWithOptions->length == 0) {
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("tasks"), array<any>()}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("tasks"), std:("No pending choices for the moment.")}
                    }}, 
                    object::pair{std:("text"), std:("No pending choices for the moment.")}
                };
            }
            shared output = std:("# Pending Tasks\
\
");
            output += std:("The following tasks are awaiting your selection:\
\
");
            tasksWithOptions->forEach([=](auto task, auto index) mutable
            {
                output += string_empty + (index + 1) + std:(". **") + task["name"] + std:("**\
");
                if (task["description"]) {
                    output += std:("   ") + task["description"] + std:("\
");
                }
                if (task["metadata"]["options"]) {
                    output += std:("   Options:\
");
                    shared options = as<any>(task["metadata"]["options"]);
                    options->forEach([=](auto option) mutable
                    {
                        if (type_of(option) == std:("string")) {
                            auto description = OR((task["metadata"]["options"]["find"]([=](auto o) mutable
                            {
                                return o["name"] == option;
                            }
                            )["description"]), (string_empty));
                            output += std:("   - "") + option + std:("" ") + (description) ? any(std:("- ") + description + string_empty) (string_empty) + std:("\
");
                        } else {
                            output += std:("   - "") + option["name"] + std:("" ") + (option["description"]) ? any(std:("- ") + option["description"] + string_empty) (string_empty) + std:("\
");
                        }
                    }
                    );
                }
                output += std:("\
");
            }
            );
            output += std:("To select an option, reply with the option name (e.g., 'post' or 'cancel').\
");
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("tasks"), tasksWithOptions}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("tasks"), output}
                }}, 
                object::pair{std:("text"), output}
            };
        }
        catch (const any& error)
        {
            logger->error(object{
                object::pair{std:("error"), std:("error")}
            }, std:("Error in options provider:"));
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("tasks"), array<any>()}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("tasks"), std:("There was an error retrieving pending tasks with options.")}
                }}, 
                object::pair{std:("text"), std:("There was an error retrieving pending tasks with options.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
