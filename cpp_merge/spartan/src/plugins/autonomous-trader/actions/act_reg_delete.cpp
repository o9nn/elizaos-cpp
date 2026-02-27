#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_reg_delete.h"

std::shared_ptr<Action> deleteRegistration = object{
    object::pair{std:("name"), std:("DELETE_REGISTRATION")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        if (!message->metadata->authorId) return false;
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        return email;
    }
    }, 
    object::pair{std:("description"), std:("Allows a user to delete their account with Spartan services")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("DELETE_REGISTRATION handler"));
        auto roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        console->log(std:("entity"), entity);
        auto existingComponent = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (existingComponent) {
            console->log(std:("deleting"), existingComponent);
            takeItPrivate(runtime, message, std:("Just cleared your registration: ") + existingComponent->data->address);
            runtime->deleteComponent(existingComponent->id);
        } else {
            takeItPrivate(runtime, message, std:("Cant find your registration"));
        }
        responses->get_length() = 0;
        return std::shared_ptr<Promise<boolean>>();
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Please delete my registration")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help you delete your registration")}, 
            object::pair{std:("actions"), array<string>{ std:("DELETE_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I can I delete my registration")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Yes that's available")}, 
            object::pair{std:("thought"), std:("User is curious but we want confirmed before we act")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Please delete my email on openai")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Please delete my signup on user@email.com")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help you delete your registration")}, 
            object::pair{std:("actions"), array<string>{ std:("DELETE_REGISTRATION") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
