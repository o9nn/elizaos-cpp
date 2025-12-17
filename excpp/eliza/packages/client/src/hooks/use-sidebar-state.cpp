#include "use-sidebar-state.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useSidebarState() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [isVisible, setIsVisible] = useState<boolean>(false);

    // Load sidebar state from localStorage on mount
    useEffect(() => {
        try {
            const auto stored = localStorage.getItem(SIDEBAR_STATE_KEY);
            if (stored != null) {
                const auto parsedState = JSON.parse(stored);
                setIsVisible(parsedState);
            }
            } catch (error) {
                clientLogger.error('Error reading sidebar state from localStorage:', error);
                // Default to false if there's an error
                setIsVisible(false);
            }
            }, []);

            // Update sidebar state and persist to localStorage
            const auto setSidebarVisible = useCallback((visible: boolean) => {;
                setIsVisible(visible);

                try {
                    localStorage.setItem(SIDEBAR_STATE_KEY, JSON.stringify(visible));
                    } catch (error) {
                        clientLogger.error('Error saving sidebar state to localStorage:', error);
                    }
                    }, []);

                    // Toggle function for convenience
                    const auto toggleSidebar = useCallback(() => {;
                        setSidebarVisible(!isVisible);
                        }, [isVisible, setSidebarVisible]);

                        return {
                            isVisible,
                            setSidebarVisible,
                            toggleSidebar,
                            };

}

} // namespace elizaos
