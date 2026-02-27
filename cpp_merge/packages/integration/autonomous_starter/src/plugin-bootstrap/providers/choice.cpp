#include "choice.hpp"
#include <string>

std::shared_ptr<Provider> choiceProvider = object{
    object::pair{std::string("name"), std::string("CHOICE")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message) mutable
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
                auto options = as<array<any>>(task["metadata"]["options"]);
                if (options->every([=](auto opt) mutable
                {
                    return type_of(opt) == std::string("string");
                }
                )) {
                    (as<array<string>>(options))->forEach([=](auto optionName) mutable
                    {
                        output += std::string("   - "") + optionName + std::string(""\
");
                    }
                    );
                } else if (options->every([=](auto opt) mutable
                {
                    return AND((AND((type_of(opt) == std::string("object")), (opt != nullptr))), (in(std::string("name"), opt)));
                }
                )) {
                    (as<array<std::shared_ptr<OptionObject>>>(options))->forEach([=](auto optionObj) mutable
                    {
                        output += std::string("   - "") + optionObj->name + std::string("" ") + (optionObj->description) ? any(std::string("- ") + optionObj->description + string_empty) (string_empty) + std::string("\
");
                    }
                    );
                } else {
                    logger->warn(std::string("[choiceProvider] Task ") + task["id"] + std::string(" has options in an unexpected format."));
                    output += std::string("   - Options format unclear for this task.\
");
                }
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
    }
};

void Main(void)
{
}

MAIN
