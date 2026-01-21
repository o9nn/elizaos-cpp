#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/facts.h"

any formatFacts(array<std::shared_ptr<Memory>> facts)
{
    return facts->reverse()->map([=](auto fact) mutable
    {
        return fact->content->text;
    }
    )->join(std::string("\
"));
};


std::shared_ptr<Provider> factsProvider = object{
    object::pair{std::string("name"), std::string("FACTS")}, 
    object::pair{std::string("description"), std::string("Key facts that the agent knows")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto recentMessages = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("tableName"), std::string("messages")}, 
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("count"), 10}, 
            object::pair{std::string("unique"), false}
        }); });
        auto last5Messages = recentMessages->slice(-5)->map([=](auto message) mutable
        {
            return message["content"]->text;
        }
        )->join(std::string("\
"));
        auto embedding = std::async([=]() { runtime->useModel(ModelType->TEXT_EMBEDDING, object{
            object::pair{std::string("text"), last5Messages}
        }); });
        auto [relevantFacts, recentFactsData] = std::async([=]() { Promise->all(std::tuple<any, any>{ runtime->searchMemories(object{
            object::pair{std::string("tableName"), std::string("facts")}, 
            object::pair{std::string("embedding"), std::string("embedding")}, 
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("worldId"), message->worldId}, 
            object::pair{std::string("count"), 6}, 
            object::pair{std::string("query"), message->content->text}
        }), runtime->searchMemories(object{
            object::pair{std::string("embedding"), std::string("embedding")}, 
            object::pair{std::string("query"), message->content->text}, 
            object::pair{std::string("tableName"), std::string("facts")}, 
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("entityId"), message->entityId}, 
            object::pair{std::string("count"), 6}
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
                object::pair{std::string("values"), object{
                    object::pair{std::string("facts"), string_empty}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("facts"), allFacts}
                }}, 
                object::pair{std::string("text"), string_empty}
            };
        }
        auto formattedFacts = formatFacts(allFacts);
        shared text = std::string("Key facts that {{agentName}} knows:\
{{formattedFacts}}")->replace(std::string("{{agentName}}"), runtime->character->name)->replace(std::string("{{formattedFacts}}"), formattedFacts);
        return object{
            object::pair{std::string("values"), object{
                object::pair{std::string("facts"), formattedFacts}
            }}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("facts"), allFacts}
            }}, 
            object::pair{std::string("text"), std::string("text")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
