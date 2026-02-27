#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/embedding-seed.h"

std::function<any(double)> fixedUuid = [=](auto n) mutable
{
    return string_empty + std::string("0")->repeat(8) + std::string("-") + std::string("0")->repeat(4) + std::string("-") + std::string("0")->repeat(4) + std::string("-") + std::string("0")->repeat(4) + std::string("-") + n->toString()->padStart(12, std::string("0")) + string_empty;
};
any embeddingTestAgentId = fixedUuid(1);
any embeddingTestRoomId = fixedUuid(2);
any embeddingTestEntityId = fixedUuid(3);
any embeddingTestWorldId = fixedUuid(4);
std::function<array<double>(double)> generateRandomVector = [=](auto size) mutable
{
    return Array->from(object{
        object::pair{std::string("length"), size}
    }, [=]() mutable
    {
        return (Math->random() * 2 - 1) * 0.1;
    }
    );
};
any embeddingTestAgent = as<std::shared_ptr<Agent>>(object{
    object::pair{std::string("id"), embeddingTestAgentId}, 
    object::pair{std::string("name"), std::string("Embedding Test Agent")}, 
    object::pair{std::string("username"), std::string("embedding_test_agent")}, 
    object::pair{std::string("system"), std::string("Test agent system prompt")}, 
    object::pair{std::string("bio"), std::string("An agent for testing embedding functionality")}, 
    object::pair{std::string("enabled"), true}, 
    object::pair{std::string("status"), AgentStatus->ACTIVE}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("messageExamples"), array<any>()}, 
    object::pair{std::string("postExamples"), array<any>()}, 
    object::pair{std::string("topics"), array<any>()}, 
    object::pair{std::string("adjectives"), array<any>()}, 
    object::pair{std::string("knowledge"), array<any>()}, 
    object::pair{std::string("plugins"), array<any>()}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("dummySetting"), std::string("dummy value")}
    }}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("all"), array<any>()}, 
        object::pair{std::string("chat"), array<any>()}, 
        object::pair{std::string("post"), array<any>()}
    }}
});
std::shared_ptr<Entity> embeddingTestEntity = object{
    object::pair{std::string("id"), embeddingTestEntityId}, 
    object::pair{std::string("names"), array<string>{ std::string("Test Entity") }}, 
    object::pair{std::string("agentId"), embeddingTestAgentId}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("description"), std::string("A test entity for embedding tests")}
    }}
};
std::shared_ptr<Room> embeddingTestRoom = object{
    object::pair{std::string("id"), embeddingTestRoomId}, 
    object::pair{std::string("name"), std::string("Embedding Test Room")}, 
    object::pair{std::string("agentId"), embeddingTestAgentId}, 
    object::pair{std::string("source"), std::string("test")}, 
    object::pair{std::string("type"), ChannelType->DM}, 
    object::pair{std::string("worldId"), embeddingTestWorldId}
};
array<std::shared_ptr<TestMemory>> embeddingTestMemories = array<std::shared_ptr<TestMemory>>{ object{
    object::pair{std::string("id"), fixedUuid(10)}, 
    object::pair{std::string("entityId"), embeddingTestEntityId}, 
    object::pair{std::string("agentId"), embeddingTestAgentId}, 
    object::pair{std::string("roomId"), embeddingTestRoomId}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is test memory 1")}, 
        object::pair{std::string("type"), std::string("text")}
    }}, 
    object::pair{std::string("unique"), true}, 
    object::pair{std::string("type"), std::string("test")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("test")}
    }}
}, object{
    object::pair{std::string("id"), fixedUuid(11)}, 
    object::pair{std::string("entityId"), embeddingTestEntityId}, 
    object::pair{std::string("agentId"), embeddingTestAgentId}, 
    object::pair{std::string("roomId"), embeddingTestRoomId}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is test memory 2")}, 
        object::pair{std::string("type"), std::string("text")}
    }}, 
    object::pair{std::string("unique"), true}, 
    object::pair{std::string("type"), std::string("test")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("test")}
    }}
}, object{
    object::pair{std::string("id"), fixedUuid(12)}, 
    object::pair{std::string("entityId"), embeddingTestEntityId}, 
    object::pair{std::string("agentId"), embeddingTestAgentId}, 
    object::pair{std::string("roomId"), embeddingTestRoomId}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is test memory 3")}, 
        object::pair{std::string("type"), std::string("text")}
    }}, 
    object::pair{std::string("unique"), true}, 
    object::pair{std::string("type"), std::string("test")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("test")}
    }}
} };
array<any> embeddingTestData = array<any>{ object{
    object::pair{std::string("id"), fixedUuid(30)}, 
    object::pair{std::string("memoryId"), as<std::shared_ptr<UUID>>(const_(embeddingTestMemories)[0]->id)}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("dim384"), generateRandomVector(384)}
}, object{
    object::pair{std::string("id"), fixedUuid(31)}, 
    object::pair{std::string("memoryId"), as<std::shared_ptr<UUID>>(const_(embeddingTestMemories)[1]->id)}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("dim512"), generateRandomVector(512)}
}, object{
    object::pair{std::string("id"), fixedUuid(32)}, 
    object::pair{std::string("memoryId"), as<std::shared_ptr<UUID>>(const_(embeddingTestMemories)[2]->id)}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("dim768"), generateRandomVector(768)}
} };
array<any> embeddingTestMemoriesWithEmbedding = array<any>{ utils::assign(object{
    , 
    object::pair{std::string("embedding"), const_(embeddingTestData)[0]["dim384"]}, 
    object::pair{std::string("metadata"), const_(embeddingTestMemories)[0]->metadata}
}, const_(embeddingTestMemories)[0]), utils::assign(object{
    , 
    object::pair{std::string("embedding"), const_(embeddingTestData)[1]["dim512"]}, 
    object::pair{std::string("metadata"), const_(embeddingTestMemories)[1]->metadata}
}, const_(embeddingTestMemories)[1]), utils::assign(object{
    , 
    object::pair{std::string("embedding"), const_(embeddingTestData)[2]["dim768"]}, 
    object::pair{std::string("metadata"), const_(embeddingTestMemories)[2]->metadata}
}, const_(embeddingTestMemories)[2]) };

void Main(void)
{
}

MAIN
