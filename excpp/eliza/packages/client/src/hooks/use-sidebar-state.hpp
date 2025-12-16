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
;

// Key for storing sidebar visibility state in localStorage
const SIDEBAR_STATE_KEY = 'eliza-agent-sidebar-visible';

/**
 * Custom hook to manage agent sidebar visibility state with localStorage persistence
 * Remembers whether the sidebar should be open or closed across page refreshes
 */

    } catch (error) {
      clientLogger.error('Error reading sidebar state from localStorage:', error);
      // Default to false if there's an error
      setIsVisible(false);
    }
  }, []);

  // Update sidebar state and persist to localStorage
  const setSidebarVisible = useCallback((visible: boolean) => {
    setIsVisible(visible);

    try {
      localStorage.setItem(SIDEBAR_STATE_KEY, JSON.stringify(visible));
    } catch (error) {
      clientLogger.error('Error saving sidebar state to localStorage:', error);
    }
  }, []);

  // Toggle , [isVisible, setSidebarVisible]);

  return {
    isVisible,
    setSidebarVisible,
    toggleSidebar,
  };
}

} // namespace elizaos
