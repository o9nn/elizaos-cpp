#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/hooks/use-agent-tab-state.h"

any useAgentTabState(any agentId)
{
    auto [currentTab, setCurrentTab] = useState<TabValue>(std::string("details"));
    shared getStoredTabStates = useCallback([=]() mutable
    {
        try
        {
            auto stored = localStorage->getItem(AGENT_TAB_STATE_KEY);
            return (stored) ? any(JSON->parse(stored)) : any(object{});
        }
        catch (const any& error)
        {
            clientLogger->error(std::string("Error reading agent tab states from localStorage:"), error);
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
            clientLogger->error(std::string("Error saving agent tab states to localStorage:"), error);
        }
    }
    , array<any>());
    useEffect([=]() mutable
    {
        if (!agentId) {
            setCurrentTab(std::string("details"));
            return;
        }
        auto storedStates = getStoredTabStates();
        auto agentTabState = OR((const_(storedStates)[agentId]), (std::string("details")));
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
        object::pair{std::string("currentTab"), std::string("currentTab")}, 
        object::pair{std::string("setTab"), updateTab}
    };
};


string AGENT_TAB_STATE_KEY = std::string("eliza-agent-tab-states");

void Main(void)
{
}

MAIN
