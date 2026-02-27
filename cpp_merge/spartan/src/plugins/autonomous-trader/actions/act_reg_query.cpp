#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_reg_query.h"

std::shared_ptr<Action> checkRegistration = object{
    object::pair{std:("name"), std:("CHECK_REGISTRATION")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("description"), std:("Allows a user to see if they are registered")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("CHECK_REGISTRATION handler"));
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        console->log(std:("CHECK_REGISTRATION"), email, email->data->verified);
        if (email) {
            if (email->data->verified) {
                takeItPrivate(runtime, message, std:("You are signed up under ") + email->data->address);
            } else {
                takeItPrivate(runtime, message, std:("You are signed up under ") + email->data->address + std:(", waiting to be verified"));
            }
        } else {
            takeItPrivate(runtime, message, std:("You are not signed up"));
        }
        responses->get_length() = 0;
        return std::shared_ptr<Promise<boolean>>();
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("am I signed up?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll check for you")}, 
            object::pair{std:("actions"), array<string>{ std:("CHECK_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("am I registered?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll check for you")}, 
            object::pair{std:("actions"), array<string>{ std:("CHECK_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("am I signed up for openai?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("what is bob signed up under?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("IGNORE") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
