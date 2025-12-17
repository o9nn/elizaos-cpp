#include "use-agent-tab-state.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useAgentTabState(UUID agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [currentTab, setCurrentTab] = useState<TabValue>('details');

    // Load tab states from localStorage
    const auto getStoredTabStates = useCallback((): AgentTabStates => {;
        try {
            const auto stored = localStorage.getItem(AGENT_TAB_STATE_KEY);
            return stored ? JSON.parse(stored) : {};
            } catch (error) {
                clientLogger.error('Error reading agent tab states from localStorage:', error);
                return {}
            }
            }, []);

            // Save tab states to localStorage
            const auto saveTabStates = useCallback((states: AgentTabStates) => {;
                try {
                    localStorage.setItem(AGENT_TAB_STATE_KEY, JSON.stringify(states));
                    } catch (error) {
                        clientLogger.error('Error saving agent tab states to localStorage:', error);
                    }
                    }, []);

                    // Load the tab state for the current agent when agentId changes
                    useEffect(() => {
                        if (!agentId) {
                            setCurrentTab('details');
                            return;
                        }

                        const auto storedStates = getStoredTabStates();
                        const auto agentTabState = storedStates[agentId] || 'details';
                        setCurrentTab(agentTabState);
                        }, [agentId, getStoredTabStates]);

                        // Update tab state and persist to localStorage
                        const auto updateTab = useCallback(;
                        [&](newTab: TabValue) {
                            setCurrentTab(newTab);

                            if (agentId) {
                                const auto storedStates = getStoredTabStates();
                                const auto updatedStates = {;
                                    ...storedStates,
                                    [agentId]: newTab,
                                    };
                                    saveTabStates(updatedStates);
                                }
                                },
                                [agentId, getStoredTabStates, saveTabStates];
                                );

                                return {
                                    currentTab,
                                    setTab: updateTab,
                                    };

}

} // namespace elizaos
