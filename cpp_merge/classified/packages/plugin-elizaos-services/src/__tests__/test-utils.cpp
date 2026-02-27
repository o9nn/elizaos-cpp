#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/__tests__/test-utils.h"

std::shared_ptr<MockRuntime> createMockRuntime(Partial<std::shared_ptr<MockRuntime>> overrides)
{
    auto mockRuntime = utils::assign(object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("Test Character")}, 
            object::pair{std:("bio"), std:("This is a test character for testing")}
        }}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("getService"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std:("registerService"), mock()}, 
        object::pair{std:("getSetting"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std:("useModel"), mock()->mockImplementation([=](auto modelType, auto params) mutable
        {
            if (modelType == ModelType->TEXT_SMALL) {
                return Promise->resolve(std:("Never gonna give you up, never gonna let you down"));
            } else if (modelType == ModelType->TEXT_LARGE) {
                return Promise->resolve(std:("Never gonna make you cry, never gonna say goodbye"));
            } else if (modelType == ModelType->OBJECT_LARGE) {
                return Promise->resolve(object{
                    object::pair{std:("thought"), std:("I should respond in a friendly way")}, 
                    object::pair{std:("message"), std:("Hello there! How can I help you today?")}
                });
            }
            return Promise->resolve(std:("Default response"));
        }
        )}, 
        object::pair{std:("init"), mock()->mockResolvedValue(undefined)}
    }, overrides);
    return mockRuntime;
};


Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-message-id"))}, 
        object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
        object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-entity-id"))}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
        object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
            object::pair{std:("text"), std:("Test message")}, 
            object::pair{std:("source"), std:("test")}
        })}, 
        object::pair{std:("createdAt"), Date->now()}
    }, overrides);
};


Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        , 
        object::pair{std:("values"), utils::assign(object{
            object::pair{std:("recentMessages"), std:("User: Test message")}
        }, overrides->values)}, 
        object::pair{std:("data"), utils::assign(object{
        }, overrides->data)}
    }, overrides);
};


any setupTest(object overrides)
{
    auto callbackFn = mock();
    auto mockMessage = createMockMemory(overrides["messageOverrides"]);
    auto mockState = createMockState(overrides["stateOverrides"]);
    auto mockRuntime = createMockRuntime(utils::assign(object{
    }, overrides["runtimeOverrides"]));
    return object{
        object::pair{std:("mockRuntime"), std:("mockRuntime")}, 
        object::pair{std:("mockMessage"), std:("mockMessage")}, 
        object::pair{std:("mockState"), std:("mockState")}, 
        object::pair{std:("callbackFn"), std:("callbackFn")}
    };
};


any setupLoggerSpies()
{
    spyOn(logger, std:("info"))->mockImplementation([=]() mutable
    {
    }
    );
    spyOn(logger, std:("error"))->mockImplementation([=]() mutable
    {
    }
    );
    spyOn(logger, std:("warn"))->mockImplementation([=]() mutable
    {
    }
    );
    spyOn(logger, std:("debug"))->mockImplementation([=]() mutable
    {
    }
    );
    return [=]() mutable
    {
        return mock->restore();
    };
};


