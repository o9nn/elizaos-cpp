#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/hooks/use-agent-tab-state.h"

any useAgentTabState(any agentId)
{
    auto [currentTab, setCurrentTab] = useState<TabValue>(std:("details"));
    shared getStoredTabStates = useCallback([=]() mutable
    {
        try
        {
            auto stored = localStorage->getItem(AGENT_TAB_STATE_KEY);
            return (stored) ? any(JSON->parse(stored)) (object{});
        }
        catch (const any& error)
        {
            clientLogger->error(std:("Error reading agent tab states from localStorage:"), error);
            return object{};
        }
    }
    , array<any>());
    shared saveTabStates = useCallback([=](auto states) mutable
    {
        try
        {
            localStorage->setItem(AGENT_TAB_STATE_KEY, JSON->stringify(states));
        }
        catch (const any& error)
        {
            clientLogger->error(std:("Error saving agent tab states to localStorage:"), error);
        }
    }
    , array<any>());
    useEffect([=]() mutable
    {
        if (!agentId) {
            setCurrentTab(std:("details"));
            return;
        }
        auto storedStates = getStoredTabStates();
        auto agentTabState = OR((const_(storedStates)[agentId]), (std:("details")));
        setCurrentTab(agentTabState);
    }
    , array<any>{ agentId, getStoredTabStates });
    auto updateTab = useCallback([=](auto newTab) mutable
    {
        setCurrentTab(newTab);
        if (agentId) {
            auto storedStates = getStoredTabStates();
            auto updatedStates = utils::assign(object{
                , 
                object::pair{agentId, newTab}
            }, storedStates);
            saveTabStates(updatedStates);
        }
    }
    , array<any>{ agentId, getStoredTabStates, saveTabStates });
    return object{
        object::pair{std:("currentTab"), std:("currentTab")}, 
        object::pair{std:("setTab"), updateTab}
    };
};


string AGENT_TAB_STATE_KEY = std:("eliza-agent-tab-states");

void Main(void)
{
}

MAIN
