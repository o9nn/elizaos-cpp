#include "test-utils.h"
#include <string>

std::shared_ptr<MockRuntime> createMockRuntime(Partial<std::shared_ptr<MockRuntime>> overrides)
{
    auto mockRuntime = utils::assign(object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("Test Character")}, 
            object::pair{std::string("bio"), std::string("This is a test character for testing")}
        }}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("getService"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std::string("registerService"), mock()}, 
        object::pair{std::string("getSetting"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std::string("useModel"), mock()->mockImplementation([=](auto modelType, auto params) mutable
        {
            if (modelType == ModelType->TEXT_SMALL) {
                return Promise->resolve(std::string("Never gonna give you up, never gonna let you down"));
            } else if (modelType == ModelType->TEXT_LARGE) {
                return Promise->resolve(std::string("Never gonna make you cry, never gonna say goodbye"));
            } else if (modelType == ModelType->OBJECT_LARGE) {
                return Promise->resolve(object{
                    object::pair{std::string("thought"), std::string("I should respond in a friendly way")}, 
                    object::pair{std::string("message"), std::string("Hello there! How can I help you today?")}
                });
            }
            return Promise->resolve(std::string("Default response"));
        }
        )}, 
        object::pair{std::string("init"), mock()->mockResolvedValue(std::nullopt)}
    }, overrides);
    return mockRuntime;
};


Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-message-id"))}, 
        object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
        object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-entity-id"))}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
        object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
            object::pair{std::string("text"), std::string("Test message")}, 
            object::pair{std::string("source"), std::string("test")}
        })}, 
        object::pair{std::string("createdAt"), Date->now()}
    }, overrides);
};


Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        , 
        object::pair{std::string("values"), utils::assign(object{
            object::pair{std::string("recentMessages"), std::string("User: Test message")}
        }, overrides->values)}, 
        object::pair{std::string("data"), utils::assign(object{
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
        object::pair{std::string("mockRuntime"), std::string("mockRuntime")}, 
        object::pair{std::string("mockMessage"), std::string("mockMessage")}, 
        object::pair{std::string("mockState"), std::string("mockState")}, 
        object::pair{std::string("callbackFn"), std::string("callbackFn")}
    };
};


any setupLoggerSpies()
{
    spyOn(logger, std::string("info"))->mockImplementation([=]() mutable
    {
    }
    );
    spyOn(logger, std::string("error"))->mockImplementation([=]() mutable
    {
    }
    );
    spyOn(logger, std::string("warn"))->mockImplementation([=]() mutable
    {
    }
    );
    spyOn(logger, std::string("debug"))->mockImplementation([=]() mutable
    {
    }
    );
    return [=]() mutable
    {
        return mock->restore();
    };
};


