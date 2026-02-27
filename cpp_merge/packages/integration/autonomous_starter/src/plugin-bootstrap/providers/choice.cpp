#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/choice.h"

std::shared_ptr<Provider> choiceProvider = object{
    object::pair{std:("name"), std:("CHOICE")}, 
    object::pair{std:("get"), [=](auto runtime, auto message) mutable
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
                auto options = as<array<any>>(task["metadata"]["options"]);
                if (options->every([=](auto opt) mutable
                {
                    return type_of(opt) == std:("string");
                }
                )) {
                    (as<array<string>>(options))->forEach([=](auto optionName) mutable
                    {
                        output += std:("   - "") + optionName + std:(""\
");
                    }
                    );
                } else if (options->every([=](auto opt) mutable
                {
                    return AND((AND((type_of(opt) == std:("object")), (opt != nullptr))), (in(std:("name"), opt)));
                }
                )) {
                    (as<array<std::shared_ptr<OptionObject>>>(options))->forEach([=](auto optionObj) mutable
                    {
                        output += std:("   - "") + optionObj->name + std:("" ") + (optionObj->description) ? any(std:("- ") + optionObj->description + string_empty) (string_empty) + std:("\
");
                    }
                    );
                } else {
                    logger->warn(std:("[choiceProvider] Task ") + task["id"] + std:(" has options in an unexpected format."));
                    output += std:("   - Options format unclear for this task.\
");
                }
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
    }
};

void Main(void)
{
}

MAIN
