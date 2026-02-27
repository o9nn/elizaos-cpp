#include "facts.hpp"

any formatFacts(array<std::shared_ptr<Memory>> facts)
{
    return facts->reverse()->map([=](auto fact) mutable
    {
        return fact->content->text;
    }
    )->join(std:("\
"));
};


std::shared_ptr<Provider> factsProvider = object{
    object::pair{std:("name"), std:("FACTS")}, 
    object::pair{std:("description"), std:("Key facts that the agent knows")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        try
        {
            auto recentMessages = std::async([=]() { runtime->getMemories(object{
                object::pair{std:("tableName"), std:("messages")}, 
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("count"), 10}, 
                object::pair{std:("unique"), false}
            }); });
            auto last5Messages = recentMessages->slice(-5)->map([=](auto message) mutable
            {
                return message["content"]->text;
            }
            )->join(std:("\
"));
            auto embedding = std::async([=]() { runtime->useModel(ModelType->TEXT_EMBEDDING, object{
                object::pair{std:("text"), last5Messages}
            }); });
            auto [relevantFacts, recentFactsData] = std::async([=]() { Promise->all(std::tuple<any, any>{ runtime->searchMemories(object{
                object::pair{std:("tableName"), std:("facts")}, 
                object::pair{std:("embedding"), std:("embedding")}, 
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("worldId"), message->worldId}, 
                object::pair{std:("count"), 6}, 
                object::pair{std:("query"), message->content->text}
            }), runtime->searchMemories(object{
                object::pair{std:("embedding"), std:("embedding")}, 
                object::pair{std:("query"), message->content->text}, 
                object::pair{std:("tableName"), std:("facts")}, 
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("entityId"), message->entityId}, 
                object::pair{std:("count"), 6}
            }) }); });
            auto allFacts = (array<any>{ relevantFacts, recentFactsData })->filter([=](auto fact, auto index, auto self) mutable
            {
                return index == self->findIndex([=](auto t) mutable
                {
                    return t["id"] == fact["id"];
                }
                );
            }
            );
            if (allFacts->get_length() == 0) {
                return object{
                    object::pair{std:("values"), object{
                        object::pair{std:("facts"), string_empty}
                    }}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("facts"), allFacts}
                    }}, 
                    object::pair{std:("text"), std:("No facts available.")}
                };
            }
            auto formattedFacts = formatFacts(allFacts);
            shared text = std:("Key facts that {{agentName}} knows:\
{{formattedFacts}}")->replace(std:("{{agentName}}"), runtime->character->name)->replace(std:("{{formattedFacts}}"), formattedFacts);
            return object{
                object::pair{std:("values"), object{
                    object::pair{std:("facts"), formattedFacts}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("facts"), allFacts}
                }}, 
                object::pair{std:("text"), std:("text")}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in factsProvider:"), error);
            return object{
                object::pair{std:("values"), object{
                    object::pair{std:("facts"), string_empty}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("facts"), array<any>()}
                }}, 
                object::pair{std:("text"), std:("Error retrieving facts.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
