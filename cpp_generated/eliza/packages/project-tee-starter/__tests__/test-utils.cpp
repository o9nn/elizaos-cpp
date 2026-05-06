#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/test-utils.h"

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    auto baseRuntime = createCoreMockRuntime();
    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
        , 
        object::pair{std::string("character"), mrTeeCharacter}, 
        object::pair{std::string("plugins"), array<any>{ plugin }}, 
        object::pair{std::string("registerPlugin"), mock()}, 
        object::pair{std::string("initialize"), mock()}, 
        object::pair{std::string("getService"), mock()}, 
        object::pair{std::string("getSetting"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std::string("useModel"), mock()->mockResolvedValue(std::string("Test model response"))}, 
        object::pair{std::string("getProviderResults"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("evaluateProviders"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("evaluate"), mock()->mockResolvedValue(array<any>())}
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
    auto mockMessage = createMockMessage(OR((options["messageText"]), (std::string("Test message"))), OR((options["messageOverrides"]), (object{})));
    auto mockState = createMockState(OR((options["stateOverrides"]), (object{})));
    auto mockRuntime = createMockRuntime(OR((options["runtimeOverrides"]), (object{})));
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


