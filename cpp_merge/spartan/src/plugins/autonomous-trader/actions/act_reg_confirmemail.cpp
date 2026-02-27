#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_reg_confirmemail.h"

any findGeneratedCode(any message, any length)
{
    auto pattern = std::make_shared<RegExp>(std:("\b[A-Za-z0-9]{") + length + std:("}\b"));
    auto match = message["match"](pattern);
    return (match) ? any(const_(match)[0]) (nullptr);
};


std::shared_ptr<Action> checkRegistrationCode = object{
    object::pair{std:("name"), std:("VERIFY_REGISTRATION_CODE")}, 
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
        auto containsGeneratedCode = findGeneratedCode(message->content->text, 16);
        if (containsGeneratedCode != nullptr) {
            runtime->runtimeLogger->log(std:("VERIFY_REGISTRATION_CODE containsGeneratedCode"), type_of((containsGeneratedCode)), containsGeneratedCode);
        }
        return AND((AND((email), (containsGeneratedCode != nullptr))), (!email->data->verified));
    }
    }, 
    object::pair{std:("description"), std:("Allows a user set their email address")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("VERIFY_REGISTRATION_CODE handler"));
        auto roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        console->log(std:("VERIFY_REGISTRATION_CODE entity"), entity);
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (!email) {
            console->log(std:("shouldnt be here"));
            return std::shared_ptr<Promise<boolean>>();
        }
        auto passedCode = findGeneratedCode(message->content->text, 16);
        if (passedCode == nullptr) {
            console->log(std:("shouldnt be here"));
            return std::shared_ptr<Promise<boolean>>();
        }
        console->log(std:("VERIFY_REGISTRATION_CODE email"), email, std:("code"), passedCode);
        if (email->data->tries == undefined) email->data->tries = 0;
        if (email->data->tries > 3) {
            console->log(std:("hacker..."));
            takeItPrivate(runtime, message, std:("You can no longer validate, you must delete your registration and restart"));
            responses->get_length() = 0;
            return std::shared_ptr<Promise<boolean>>();
        }
        if (passedCode == email->data->code) {
            email->data->verified = true;
            takeItPrivate(runtime, message, std:("Looks good, you are now registered and have access to my services"));
        } else {
            email->data->tries++;
            takeItPrivate(runtime, message, std:("That does not match my records, please double check, it is case sensitive"));
        }
        responses->get_length() = 0;
        std::async([=]() { runtime->updateComponent(object{
            object::pair{std:("id"), email->id}, 
            object::pair{std:("worldId"), roomDetails->worldId}, 
            object::pair{std:("roomId"), message->roomId}, 
            object::pair{std:("sourceEntityId"), message->entityId}, 
            object::pair{std:("entityId"), entityId}, 
            object::pair{std:("type"), EMAIL_TYPE}, 
            object::pair{std:("data"), email->data}, 
            object::pair{std:("agentId"), runtime->agentId}
        }); });
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("This is my code you sent CODE")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll check it to see if it's correct")}, 
            object::pair{std:("actions"), array<string>{ std:("VERIFY_REGISTRATION_CODE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("This is the code openai sent CODE")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("what was the code you emailed me?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'm not going to tell you")}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
