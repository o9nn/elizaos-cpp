#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
import type { UUID } from '@elizaos/core';
;

// Key for storing agent tab states in localStorage
const AGENT_TAB_STATE_KEY = 'eliza-agent-tab-states';

using TabValue = std::variant<'details', 'actions', 'logs', 'memories', std::string>;

struct AgentTabStates {
};


/**
 * Custom hook to manage agent sidebar tab state with localStorage persistence
 * Each agent remembers its last selected tab when switching between agents
 */
;
    } catch (error) {
      clientLogger.error('Error reading agent tab states from localStorage:', error);
      return {};
    }
  }, []);

  // Save tab states to localStorage
  const saveTabStates = useCallback((states: AgentTabStates) => {
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

    const storedStates = getStoredTabStates();
    const agentTabState = storedStates[agentId] || 'details';
    setCurrentTab(agentTabState);
  }, [agentId, getStoredTabStates]);

  // Update tab state and persist to localStorage
  const updateTab = useCallback(
    (newTab: TabValue) => {
      setCurrentTab(newTab);

      if (agentId) {
        const storedStates = getStoredTabStates();
        const updatedStates = {
          ...storedStates,
          [agentId]: newTab,
        };
        saveTabStates(updatedStates);
      }
    },
    [agentId, getStoredTabStates, saveTabStates]
  );

  return {
    currentTab,
    setTab: updateTab,
  };
}

} // namespace elizaos
