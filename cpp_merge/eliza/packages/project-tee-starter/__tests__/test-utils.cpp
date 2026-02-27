#include "test-utils.h"

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    auto baseRuntime = createCoreMockRuntime();
    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
        , 
        object::pair{std:("character"), mrTeeCharacter}, 
        object::pair{std:("plugins"), array<any>{ plugin }}, 
        object::pair{std:("registerPlugin"), mock()}, 
        object::pair{std:("initialize"), mock()}, 
        object::pair{std:("getService"), mock()}, 
        object::pair{std:("getSetting"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std:("useModel"), mock()->mockResolvedValue(std:("Test model response"))}, 
        object::pair{std:("getProviderResults"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("evaluateProviders"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("evaluate"), mock()->mockResolvedValue(array<any>())}
    }, baseRuntime, overrides)));
    return mockRuntime;
};


std::shared_ptr<Memory> createMockMessage(string text, Partial<std::shared_ptr<Memory>> overrides)
{
    auto baseMessage = createCoreMockMessage(text);
    return utils::assign(object{
    }, baseMessage, overrides);
};


std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    auto baseState = createCoreMockState();
    return utils::assign(object{
    }, baseState, overrides);
};


any setupTest(object options)
{
    auto callbackFn = mock();
    auto mockMessage = createMockMessage(OR((options["messageText"]), (std:("Test message"))), OR((options["messageOverrides"]), (object{})));
    auto mockState = createMockState(OR((options["stateOverrides"]), (object{})));
    auto mockRuntime = createMockRuntime(OR((options["runtimeOverrides"]), (object{})));
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


