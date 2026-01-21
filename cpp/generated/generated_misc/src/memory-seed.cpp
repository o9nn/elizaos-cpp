#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/memory-seed.h"

any memoryTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any memoryTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any memoryTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any memoryTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> memoryTestAgent = object{
    object::pair{std::string("id"), memoryTestAgentId}, 
    object::pair{std::string("name"), std::string("Memory Test Agent")}, 
    object::pair{std::string("bio"), std::string("Test agent for memory integration tests")}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("profile"), object{
            object::pair{std::string("short_description"), std::string("Test agent for memory integration tests")}
        }}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> memoryTestEntity = object{
    object::pair{std::string("id"), memoryTestEntityId}, 
    object::pair{std::string("names"), array<string>{ std::string("Memory Test Entity") }}, 
    object::pair{std::string("agentId"), memoryTestAgentId}, 
    object::pair{std::string("components"), array<any>()}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("user")}, 
        object::pair{std::string("worldId"), memoryTestWorldId}
    }}
};
std::shared_ptr<World> memoryTestWorld = object{
    object::pair{std::string("id"), memoryTestWorldId}, 
    object::pair{std::string("agentId"), memoryTestAgentId}, 
    object::pair{std::string("name"), std::string("Memory Test World")}, 
    object::pair{std::string("serverId"), std::string("test-server")}, 
    object::pair{std::string("metadata"), object{}}
};
std::shared_ptr<Room> memoryTestRoom = object{
    object::pair{std::string("id"), memoryTestRoomId}, 
    object::pair{std::string("name"), std::string("Memory Test Room")}, 
    object::pair{std::string("agentId"), memoryTestAgentId}, 
    object::pair{std::string("source"), std::string("test")}, 
    object::pair{std::string("type"), ChannelType->GROUP}, 
    object::pair{std::string("worldId"), memoryTestWorldId}, 
    object::pair{std::string("metadata"), object{}}
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
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("entityId"), memoryTestEntityId}, 
    object::pair{std::string("roomId"), memoryTestRoomId}, 
    object::pair{std::string("agentId"), memoryTestAgentId}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime() - 3600000}, 
    object::pair{std::string("unique"), true}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is a test memory for integration testing #1")}, 
        object::pair{std::string("type"), std::string("text")}
    }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("chat")}, 
        object::pair{std::string("source"), std::string("integration-test")}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("entityId"), memoryTestEntityId}, 
    object::pair{std::string("roomId"), memoryTestRoomId}, 
    object::pair{std::string("agentId"), memoryTestAgentId}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime() - 7200000}, 
    object::pair{std::string("unique"), true}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is a test memory for integration testing #2")}, 
        object::pair{std::string("type"), std::string("text")}
    }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("chat")}, 
        object::pair{std::string("source"), std::string("integration-test")}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("entityId"), memoryTestEntityId}, 
    object::pair{std::string("roomId"), memoryTestRoomId}, 
    object::pair{std::string("agentId"), memoryTestAgentId}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime() - 10800000}, 
    object::pair{std::string("unique"), true}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is a test memory for integration testing #3")}, 
        object::pair{std::string("type"), std::string("text")}
    }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("chat")}, 
        object::pair{std::string("source"), std::string("integration-test")}
    }}
} };
array<std::shared_ptr<Memory>> memoryTestMemoriesWithEmbedding = array<std::shared_ptr<Memory>>{ utils::assign(object{
    , 
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("embedding"), generateEmbedding(384)}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is a test memory with embedding dimension 384")}, 
        object::pair{std::string("type"), std::string("text")}
    }}
}, const_(memoryTestMemories)[0]), utils::assign(object{
    , 
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("embedding"), generateEmbedding(384)}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is a test memory with embedding dimension 384")}, 
        object::pair{std::string("type"), std::string("text")}
    }}
}, const_(memoryTestMemories)[1]), utils::assign(object{
    , 
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("embedding"), generateEmbedding(384)}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is a test memory with embedding dimension 384")}, 
        object::pair{std::string("type"), std::string("text")}
    }}
}, const_(memoryTestMemories)[2]) };
any documentMemoryId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Memory> memoryTestDocument = object{
    object::pair{std::string("id"), documentMemoryId}, 
    object::pair{std::string("entityId"), memoryTestEntityId}, 
    object::pair{std::string("roomId"), memoryTestRoomId}, 
    object::pair{std::string("agentId"), memoryTestAgentId}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("unique"), true}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("This is a test document memory")}, 
        object::pair{std::string("type"), std::string("text")}
    }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("document")}, 
        object::pair{std::string("documentId"), documentMemoryId}, 
        object::pair{std::string("timestamp"), Date->now()}, 
        object::pair{std::string("title"), std::string("Test Document")}, 
        object::pair{std::string("source"), std::string("integration-test")}
    }}
};
array<std::shared_ptr<Memory>> memoryTestFragments = Array(3)->fill(0)->map([=](auto _, auto index) mutable
{
    return (object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std::string("entityId"), memoryTestEntityId}, 
        object::pair{std::string("roomId"), memoryTestRoomId}, 
        object::pair{std::string("agentId"), memoryTestAgentId}, 
        object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
        object::pair{std::string("unique"), true}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("This is fragment ") + (index + 1) + std::string(" of the test document")}, 
            object::pair{std::string("type"), std::string("text")}
        }}, 
        object::pair{std::string("embedding"), generateEmbedding(384)}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("type"), std::string("fragment")}, 
            object::pair{std::string("documentId"), documentMemoryId}, 
            object::pair{std::string("position"), index}, 
            object::pair{std::string("source"), std::string("integration-test")}
        }}
    });
}
);
std::function<any(any, double)> createSimilarMemoryVector = [=](auto baseMemory, auto similarity) mutable
{
    if (OR((!baseMemory->embedding), (!Array->isArray(baseMemory->embedding)))) {
        throw any(std::make_shared<Error>(std::string("Base memory must have an embedding")));
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
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std::string("embedding"), normalizedVector}, 
        object::pair{std::string("content"), utils::assign(object{
            , 
            object::pair{std::string("text"), std::string("Similar to: ") + baseMemory->content->text + string_empty}
        }, baseMemory->content)}
    }, baseMemory);
};

void Main(void)
{
}

MAIN
