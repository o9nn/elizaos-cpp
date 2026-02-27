#include "memory-seed.h"

any memoryTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any memoryTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any memoryTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any memoryTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> memoryTestAgent = object{
    object::pair{std:("id"), memoryTestAgentId}, 
    object::pair{std:("name"), std:("Memory Test Agent")}, 
    object::pair{std:("bio"), std:("Test agent for memory integration tests")}, 
    object::pair{std:("settings"), object{
        object::pair{std:("profile"), object{
            object::pair{std:("short_description"), std:("Test agent for memory integration tests")}
        }}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> memoryTestEntity = object{
    object::pair{std:("id"), memoryTestEntityId}, 
    object::pair{std:("names"), array<string>{ std:("Memory Test Entity") }}, 
    object::pair{std:("agentId"), memoryTestAgentId}, 
    object::pair{std:("components"), array<any>()}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("user")}, 
        object::pair{std:("worldId"), memoryTestWorldId}
    }}
};
std::shared_ptr<World> memoryTestWorld = object{
    object::pair{std:("id"), memoryTestWorldId}, 
    object::pair{std:("agentId"), memoryTestAgentId}, 
    object::pair{std:("name"), std:("Memory Test World")}, 
    object::pair{std:("serverId"), std:("test-server")}, 
    object::pair{std:("metadata"), object{}}
};
std::shared_ptr<Room> memoryTestRoom = object{
    object::pair{std:("id"), memoryTestRoomId}, 
    object::pair{std:("name"), std:("Memory Test Room")}, 
    object::pair{std:("agentId"), memoryTestAgentId}, 
    object::pair{std:("source"), std:("test")}, 
    object::pair{std:("type"), ChannelType->GROUP}, 
    object::pair{std:("worldId"), memoryTestWorldId}, 
    object::pair{std:("metadata"), object{}}
};
std::function<array<double>(double)> generateEmbedding = [=](auto dimension = 384) mutable
{
    auto vector = Array(dimension)->fill(0)->map([=]() mutable
    {
        return Math->random() * 2 - 1;
    }
    );
    shared magnitude = Math->sqrt(vector->reduce([=](auto sum, auto val) mutable
    {
        return sum + val * val;
    }
    , 0));
    return vector->map([=](auto val) mutable
    {
        return Number((val / magnitude)->toFixed(6));
    }
    );
};
array<std::shared_ptr<Memory>> memoryTestMemories = array<std::shared_ptr<Memory>>{ object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("entityId"), memoryTestEntityId}, 
    object::pair{std:("roomId"), memoryTestRoomId}, 
    object::pair{std:("agentId"), memoryTestAgentId}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime() - 3600000}, 
    object::pair{std:("unique"), true}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is a test memory for integration testing #1")}, 
        object::pair{std:("type"), std:("text")}
    }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("chat")}, 
        object::pair{std:("source"), std:("integration-test")}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("entityId"), memoryTestEntityId}, 
    object::pair{std:("roomId"), memoryTestRoomId}, 
    object::pair{std:("agentId"), memoryTestAgentId}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime() - 7200000}, 
    object::pair{std:("unique"), true}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is a test memory for integration testing #2")}, 
        object::pair{std:("type"), std:("text")}
    }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("chat")}, 
        object::pair{std:("source"), std:("integration-test")}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("entityId"), memoryTestEntityId}, 
    object::pair{std:("roomId"), memoryTestRoomId}, 
    object::pair{std:("agentId"), memoryTestAgentId}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime() - 10800000}, 
    object::pair{std:("unique"), true}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is a test memory for integration testing #3")}, 
        object::pair{std:("type"), std:("text")}
    }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("chat")}, 
        object::pair{std:("source"), std:("integration-test")}
    }}
} };
array<std::shared_ptr<Memory>> memoryTestMemoriesWithEmbedding = array<std::shared_ptr<Memory>>{ utils::assign(object{
    , 
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("embedding"), generateEmbedding(384)}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is a test memory with embedding dimension 384")}, 
        object::pair{std:("type"), std:("text")}
    }}
}, const_(memoryTestMemories)[0]), utils::assign(object{
    , 
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("embedding"), generateEmbedding(384)}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is a test memory with embedding dimension 384")}, 
        object::pair{std:("type"), std:("text")}
    }}
}, const_(memoryTestMemories)[1]), utils::assign(object{
    , 
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("embedding"), generateEmbedding(384)}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is a test memory with embedding dimension 384")}, 
        object::pair{std:("type"), std:("text")}
    }}
}, const_(memoryTestMemories)[2]) };
any documentMemoryId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Memory> memoryTestDocument = object{
    object::pair{std:("id"), documentMemoryId}, 
    object::pair{std:("entityId"), memoryTestEntityId}, 
    object::pair{std:("roomId"), memoryTestRoomId}, 
    object::pair{std:("agentId"), memoryTestAgentId}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("unique"), true}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("This is a test document memory")}, 
        object::pair{std:("type"), std:("text")}
    }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("document")}, 
        object::pair{std:("documentId"), documentMemoryId}, 
        object::pair{std:("timestamp"), Date->now()}, 
        object::pair{std:("title"), std:("Test Document")}, 
        object::pair{std:("source"), std:("integration-test")}
    }}
};
array<std::shared_ptr<Memory>> memoryTestFragments = Array(3)->fill(0)->map([=](auto _, auto index) mutable
{
    return (object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std:("entityId"), memoryTestEntityId}, 
        object::pair{std:("roomId"), memoryTestRoomId}, 
        object::pair{std:("agentId"), memoryTestAgentId}, 
        object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
        object::pair{std:("unique"), true}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("This is fragment ") + (index + 1) + std:(" of the test document")}, 
            object::pair{std:("type"), std:("text")}
        }}, 
        object::pair{std:("embedding"), generateEmbedding(384)}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("type"), std:("fragment")}, 
            object::pair{std:("documentId"), documentMemoryId}, 
            object::pair{std:("position"), index}, 
            object::pair{std:("source"), std:("integration-test")}
        }}
    });
}
);
std::function<any(any, double)> createSimilarMemoryVector = [=](auto baseMemory, auto similarity) mutable
{
    if (OR((!baseMemory->embedding), (!Array->isArray(baseMemory->embedding)))) {
        throw any(std::make_shared<Error>(std:("Base memory must have an embedding")));
    }
    auto dimension = baseMemory->embedding->length;
    shared noise = generateEmbedding(dimension);
    auto blendedVector = baseMemory->embedding->map([=](auto value, auto idx) mutable
    {
        return value * similarity + const_(noise)[idx] * (1 - similarity);
    }
    );
    shared magnitude = Math->sqrt(blendedVector->reduce([=](auto sum, auto val) mutable
    {
        return sum + val * val;
    }
    , 0));
    auto normalizedVector = blendedVector->map([=](auto val) mutable
    {
        return Number((val / magnitude)->toFixed(6));
    }
    );
    return utils::assign(object{
        , 
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std:("embedding"), normalizedVector}, 
        object::pair{std:("content"), utils::assign(object{
            , 
            object::pair{std:("text"), std:("Similar to: ") + baseMemory->content->text + string_empty}
        }, baseMemory->content)}
    }, baseMemory);
};

void Main(void)
{
}

MAIN
