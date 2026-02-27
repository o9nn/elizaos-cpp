#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/integration/seed/embedding-seed.h"

std::function<any(double)> fixedUuid = [=](auto n) mutable
{
    return string_empty + std:("0")->repeat(8) + std:("-") + std:("0")->repeat(4) + std:("-") + std:("0")->repeat(4) + std:("-") + std:("0")->repeat(4) + std:("-") + n->toString()->padStart(12, std:("0")) + string_empty;
};
any embeddingTestAgentId = fixedUuid(1);
any embeddingTestRoomId = fixedUuid(2);
any embeddingTestEntityId = fixedUuid(3);
any embeddingTestWorldId = fixedUuid(4);
std::function<array<double>(double)> generateRandomVector = [=](auto size) mutable
{
    return Array->from(object{
        object::pair{std:("length"), size}
    }, [=]() mutable
    {
        return (Math->random() * 2 - 1) * 0.1;
    }
    );
};
any embeddingTestAgent = as<std::shared_ptr<Agent>>(object{
    object::pair{std:("id"), embeddingTestAgentId}, 
    object::pair{std:("name"), std:("Embedding Test Agent")}, 
    object::pair{std:("username"), std:("embedding_test_agent")}, 
    object::pair{std:("system"), std:("Test agent system prompt")}, 
    object::pair{std:("bio"), std:("An agent for testing embedding functionality")}, 
    object::pair{std:("enabled"), true}, 
    object::pair{std:("status"), AgentStatus->ACTIVE}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("messageExamples"), array<any>()}, 
    object::pair{std:("postExamples"), array<any>()}, 
    object::pair{std:("topics"), array<any>()}, 
    object::pair{std:("adjectives"), array<any>()}, 
    object::pair{std:("knowledge"), array<any>()}, 
    object::pair{std:("plugins"), array<any>()}, 
    object::pair{std:("settings"), object{
        object::pair{std:("dummySetting"), std:("dummy value")}
    }}, 
    object::pair{std:("style"), object{
        object::pair{std:("all"), array<any>()}, 
        object::pair{std:("chat"), array<any>()}, 
        object::pair{std:("post"), array<any>()}
    }}
});
std::shared_ptr<Entity> embeddingTestEntity = object{
    object::pair{std:("id"), embeddingTestEntityId}, 
    object::pair{std:("names"), array<string>{ std:("Test Entity") }}, 
    object::pair{std:("agentId"), embeddingTestAgentId}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("description"), std:("A test entity for embedding tests")}
    }}
};
std::shared_ptr<Room> embeddingTestRoom = object{
    object::pair{std:("id"), embeddingTestRoomId}, 
    object::pair{std:("name"), std:("Embedding Test Room")}, 
    object::pair{std:("agentId"), embeddingTestAgentId}, 
    object::pair{std:("source"), std:("test")}, 
    object::pair{std:("type"), ChannelType->DM}, 
    object::pair{std:("worldId"), embeddingTestWorldId}
};
array<std::shared_ptr<TestMemory>> embeddingTestMemories = array<std::shared_ptr<TestMemory>>{ object{
    object::pair{std:("id"), fixedUuid(10)}, 
    object::pair{std:("entityId"), embeddingTestEntityId}, 
    object::pair{std:("agentId"), embeddingTestAgentId}, 
    object::pair{std:("roomId"), embeddingTestRoomId}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is test memory 1")}, 
        object::pair{std:("type"), std:("text")}
    }}, 
    object::pair{std:("unique"), true}, 
    object::pair{std:("type"), std:("test")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("test")}
    }}
}, object{
    object::pair{std:("id"), fixedUuid(11)}, 
    object::pair{std:("entityId"), embeddingTestEntityId}, 
    object::pair{std:("agentId"), embeddingTestAgentId}, 
    object::pair{std:("roomId"), embeddingTestRoomId}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is test memory 2")}, 
        object::pair{std:("type"), std:("text")}
    }}, 
    object::pair{std:("unique"), true}, 
    object::pair{std:("type"), std:("test")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("test")}
    }}
}, object{
    object::pair{std:("id"), fixedUuid(12)}, 
    object::pair{std:("entityId"), embeddingTestEntityId}, 
    object::pair{std:("agentId"), embeddingTestAgentId}, 
    object::pair{std:("roomId"), embeddingTestRoomId}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is test memory 3")}, 
        object::pair{std:("type"), std:("text")}
    }}, 
    object::pair{std:("unique"), true}, 
    object::pair{std:("type"), std:("test")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("test")}
    }}
} };
array<any> embeddingTestData = array<any>{ object{
    object::pair{std:("id"), fixedUuid(30)}, 
    object::pair{std:("memoryId"), as<std::shared_ptr<UUID>>(const_(embeddingTestMemories)[0]->id)}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("dim384"), generateRandomVector(384)}
}, object{
    object::pair{std:("id"), fixedUuid(31)}, 
    object::pair{std:("memoryId"), as<std::shared_ptr<UUID>>(const_(embeddingTestMemories)[1]->id)}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("dim512"), generateRandomVector(512)}
}, object{
    object::pair{std:("id"), fixedUuid(32)}, 
    object::pair{std:("memoryId"), as<std::shared_ptr<UUID>>(const_(embeddingTestMemories)[2]->id)}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("dim768"), generateRandomVector(768)}
} };
array<any> embeddingTestMemoriesWithEmbedding = array<any>{ utils::assign(object{
    , 
    object::pair{std:("embedding"), const_(embeddingTestData)[0]["dim384"]}, 
    object::pair{std:("metadata"), const_(embeddingTestMemories)[0]->metadata}
}, const_(embeddingTestMemories)[0]), utils::assign(object{
    , 
    object::pair{std:("embedding"), const_(embeddingTestData)[1]["dim512"]}, 
    object::pair{std:("metadata"), const_(embeddingTestMemories)[1]->metadata}
}, const_(embeddingTestMemories)[1]), utils::assign(object{
    , 
    object::pair{std:("embedding"), const_(embeddingTestData)[2]["dim768"]}, 
    object::pair{std:("metadata"), const_(embeddingTestMemories)[2]->metadata}
}, const_(embeddingTestMemories)[2]) };

void Main(void)
{
}

MAIN
