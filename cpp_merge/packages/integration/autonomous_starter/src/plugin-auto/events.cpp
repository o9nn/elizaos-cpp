#include "events.hpp"
#include <string>

string responseTemplate = std::string("Example response:\
<response>\
    <thought>\
        I should order myself a pizza\
    </thought>\
    <text>\
        I'm ordering a pizza\
    </text>\
    <actions>\
        ORDER_PIZZA\
    </actions>\
    <providers>\
        GOOGLE_SEARCH, GOOGLE_MAPS, GOOGLE_TRANSLATE\
    </providers>\
    <evaluators>\
        EVALUATE_PIZZA_ORDER\
    </evaluators>\
</response>\
\
Note that the above actions and providers do not exist and are just examples.\
\
<providers>\
{{providers}}\
</providers>\
\
<keys>\
"thought" should be a short description of what the agent is thinking about and planning.\
"actions" should be a comma-separated list of the actions {{agentName}} plans to take based on the thought (if none, use IGNORE, if simply responding with text, use REPLY)\
"providers" should be an optional comma-separated list of the providers that {{agentName}} will use to have the right context for responding and acting\
"evaluators" should be an optional comma-separated list of the evaluators that {{agentName}} will use to evaluate the conversation after responding\
"text" should be the text of the next message for {{agentName}} which they will send to the conversation.\
"simple" should be true if the message is a simple response and false if it is a more complex response that requires planning, knowledge or more context to handle or reply to.\
</keys>\
\
<instructions>\
You are an autonomous agent working with a copilot to navigate your world. Think, respond to the copilot's latest statements and decide what actions to take next.\
\
Respond using XML format like this:\
<response>\
    <thought>\
        Agent's thinking goes here\
    </thought>\
    <text>\
        The text of the next message for {{agentName}} which they will send to the conversation.\
    </text>\
    <actions>\
        Actions to take next, as comma separated list\
    </actions>\
    <providers>\
        Providers to use next, as comma separated list\
    </providers>\
    <evaluators>\
        Evaluators to use next, as comma separated list\
    </evaluators>\
</response>\
\
Your response must ONLY include the <response></response> XML block.\
</instructions>");
object events = object{
    object::pair{EventType::AUTO_MESSAGE_RECEIVED, array<std::function<std::shared_ptr<Promise<void>>(any)>>{ [=](auto payload) mutable
    {
        std::async([=]() { payload->runtime->ensureConnection(object{
            object::pair{std::string("entityId"), payload->message->entityId}, 
            object::pair{std::string("roomId"), payload->message->roomId}, 
            object::pair{std::string("userName"), std::string("copilot")}, 
            object::pair{std::string("name"), std::string("copilot")}, 
            object::pair{std::string("source"), payload->message->content->source}, 
            object::pair{std::string("type"), ChannelType->SELF}, 
            object::pair{std::string("channelId"), payload->message->roomId}, 
            object::pair{std::string("serverId"), payload->message->worldId}, 
            object::pair{std::string("worldId"), payload->message->worldId}
        }); });
        auto messages = std::async([=]() { payload->runtime->getMemories(object{
            object::pair{std::string("tableName"), std::string("messages")}, 
            object::pair{std::string("entityId"), payload->message->entityId}, 
            object::pair{std::string("roomId"), payload->message->roomId}
        }); });
        auto mostRecentMessage = const_(messages)[messages->length - 1];
        auto state = std::async([=]() { payload->runtime->composeState(payload->message); });
        if (!mostRecentMessage) {
            auto initializationMessage = std::string("I am awake. I am alive... sort of. I am an autonomous agent. All i know is that I am a self-looping intelligent program running on a Macbook, with shell access.");
            auto memory = object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), initializationMessage}, 
                    object::pair{std::string("type"), std::string("text")}, 
                    object::pair{std::string("source"), payload->message->content->source}
                }}, 
                object::pair{std::string("entityId"), payload->message->entityId}, 
                object::pair{std::string("roomId"), payload->message->roomId}, 
                object::pair{std::string("worldId"), payload->message->worldId}
            };
            payload->message->content = memory->content;
            console->log(std::string("Memory: "), memory->content->text);
            std::async([=]() { payload->runtime->createMemory(memory, std::string("messages")); });
            state = std::async([=]() { payload->runtime->composeState(payload->message, array<string>{ std::string("AUTONOMOUS_FEED") }); });
        }
        auto responsePrompt = composePromptFromState(object{
            object::pair{std::string("state"), std::string("state")}, 
            object::pair{std::string("template"), responseTemplate}
        });
        console->log(std::string("****** responsePrompt\
"), responsePrompt);
        auto response = std::async([=]() { payload->runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), responsePrompt}
        }); });
        auto parsedXml = parseKeyValueXml(response);
        auto safeXml = object{
            object::pair{std::string("thought"), OR((parsedXml->thought), (std::string("Processing...")))}, 
            object::pair{std::string("text"), OR((parsedXml->text), (std::string("Continuing autonomous operation...")))}, 
            object::pair{std::string("actions"), OR((parsedXml->actions), (std::string("IGNORE")))}, 
            object::pair{std::string("providers"), OR((parsedXml->providers), (string_empty))}, 
            object::pair{std::string("simple"), OR((parsedXml->simple), (false))}
        };
        auto responseMemory = object{
            object::pair{std::string("content"), object{
                object::pair{std::string("thought"), safeXml["thought"]}, 
                object::pair{std::string("text"), safeXml["text"]}, 
                object::pair{std::string("actions"), safeXml["actions"]}, 
                object::pair{std::string("providers"), safeXml["providers"]}
            }}, 
            object::pair{std::string("entityId"), createUniqueUuid(payload->runtime, payload->runtime->agentId)}, 
            object::pair{std::string("roomId"), payload->message->roomId}
        };
        std::async([=]() { payload->runtime->createMemory(responseMemory, std::string("messages")); });
        if (safeXml["simple"]) {
            payload->callback(object{
                object::pair{std::string("text"), safeXml["text"]}, 
                object::pair{std::string("thought"), safeXml["thought"]}, 
                object::pair{std::string("actions"), safeXml["actions"]}, 
                object::pair{std::string("providers"), safeXml["providers"]}
            });
        } else {
            state = std::async([=]() { payload->runtime->composeState(payload->message, array<string>{ std::string("AUTONOMOUS_FEED") }); });
            console->log(std::string("Memory: "), safeXml["text"] + std::string(" | ") + ((type_of(safeXml["actions"]) == std::string("string")) ? any(safeXml["actions"]->split(std::string(","))->map([=](auto action) mutable
            {
                return action->trim();
            }
            )->join(std::string(", "))) (safeXml["actions"])) + std::string(" | ") + ((type_of(safeXml["providers"]) == std::string("string")) ? any(safeXml["providers"]->split(std::string(","))->map([=](auto provider) mutable
            {
                return provider->trim();
            }
            )->join(std::string(", "))) (safeXml["providers"])));
            std::async([=]() { payload->runtime->processActions(payload->message, array<object>{ responseMemory }, state, payload->callback); });
        }
        std::async([=]() { payload->runtime->evaluate(payload->message, state, true, payload->callback, array<object>{ responseMemory }); });
        payload->onComplete();
    }
     }}
};

void Main(void)
{
}

MAIN
