#include "use-panel-width-state.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void usePanelWidthState() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [mainPanelSize, setMainPanelSize] = useState<number>(DEFAULT_MAIN_PANEL_SIZE);
    const auto [sidebarPanelSize, setSidebarPanelSize] = useState<number>(DEFAULT_SIDEBAR_PANEL_SIZE);
    const auto [isFloatingMode, setIsFloatingMode] = useState<boolean>(false);
    const auto [floatingThreshold, setFloatingThreshold] = useState<number>(DEFAULT_FLOATING_THRESHOLD);

    // Load panel sizes from localStorage on mount
    useEffect(() => {
        try {
            const auto storedMainSize = localStorage.getItem(MAIN_PANEL_WIDTH_KEY);
            const auto storedSidebarSize = localStorage.getItem(SIDEBAR_PANEL_WIDTH_KEY);
            const auto storedThreshold = localStorage.getItem(FLOATING_THRESHOLD_KEY);

            if (storedMainSize != null) {
                const auto parsedMainSize = parseFloat(storedMainSize);
                if (!isNaN(parsedMainSize) && parsedMainSize > 0 && parsedMainSize <= 100) {
                    setMainPanelSize(parsedMainSize);
                }
            }

            if (storedSidebarSize != null) {
                const auto parsedSidebarSize = parseFloat(storedSidebarSize);
                if (!isNaN(parsedSidebarSize) && parsedSidebarSize > 0 && parsedSidebarSize <= 100) {
                    setSidebarPanelSize(parsedSidebarSize);
                }
            }

            if (storedThreshold != null) {
                const auto parsedThreshold = parseFloat(storedThreshold);
                if (!isNaN(parsedThreshold) && parsedThreshold > 0) {
                    setFloatingThreshold(parsedThreshold);
                }
            }
            } catch (error) {
                clientLogger.error("Error reading panel sizes from localStorage:", error);
                // Use defaults if there's an error
                setMainPanelSize(DEFAULT_MAIN_PANEL_SIZE);
                setSidebarPanelSize(DEFAULT_SIDEBAR_PANEL_SIZE);
                setFloatingThreshold(DEFAULT_FLOATING_THRESHOLD);
            }
            }, []);

            // Resize listener to detect when to switch to floating mode
            useEffect(() => {
                const auto checkFloatingMode = [&]() {;
                    const auto shouldFloat = window.innerWidth < floatingThreshold;
                    setIsFloatingMode(shouldFloat);
                    };

                    // Check on mount
                    checkFloatingMode();

                    // Add resize listener
                    window.addEventListener("resize", checkFloatingMode);

                    return [&]() {;
                        window.removeEventListener("resize", checkFloatingMode);
                        };
                        }, [floatingThreshold]);

                        // Update main panel size and persist to localStorage
                        const auto setMainPanelSizePersistent = useCallback((size: number) => {;
                            // Validate size is within reasonable bounds
                            const auto validatedSize = Math.max(20, Math.min(80, size));
                            setMainPanelSize(validatedSize);

                            try {
                                localStorage.setItem(MAIN_PANEL_WIDTH_KEY, std::to_string(validatedSize));
                                } catch (error) {
                                    clientLogger.error("Error saving main panel size to localStorage:", error);
                                }
                                }, []);

                                // Update sidebar panel size and persist to localStorage
                                const auto setSidebarPanelSizePersistent = useCallback((size: number) => {;
                                    // Validate size is within reasonable bounds
                                    const auto validatedSize = Math.max(20, Math.min(80, size));
                                    setSidebarPanelSize(validatedSize);

                                    try {
                                        localStorage.setItem(SIDEBAR_PANEL_WIDTH_KEY, std::to_string(validatedSize));
                                        } catch (error) {
                                            clientLogger.error("Error saving sidebar panel size to localStorage:", error);
                                        }
                                        }, []);

                                        // Update floating threshold and persist to localStorage
                                        const auto setFloatingThresholdPersistent = useCallback((threshold: number) => {;
                                            const auto validatedThreshold = Math.max(320, Math.min(2560, threshold));
                                            setFloatingThreshold(validatedThreshold);

                                            try {
                                                localStorage.setItem(FLOATING_THRESHOLD_KEY, std::to_string(validatedThreshold));
                                                } catch (error) {
                                                    clientLogger.error("Error saving floating threshold to localStorage:", error);
                                                }
                                                }, []);

                                                // Manual toggle for floating mode (overrides automatic detection)
                                                const auto toggleFloatingMode = useCallback(() => {;
                                                    setIsFloatingMode((prev) => !prev);
                                                    }, []);

                                                    // Reset to default sizes
                                                    const auto resetPanelSizes = useCallback(() => {;
                                                        setMainPanelSize(DEFAULT_MAIN_PANEL_SIZE);
                                                        setSidebarPanelSize(DEFAULT_SIDEBAR_PANEL_SIZE);
                                                        setFloatingThreshold(DEFAULT_FLOATING_THRESHOLD);

                                                        try {
                                                            localStorage.removeItem(MAIN_PANEL_WIDTH_KEY);
                                                            localStorage.removeItem(SIDEBAR_PANEL_WIDTH_KEY);
                                                            localStorage.removeItem(FLOATING_THRESHOLD_KEY);
                                                            } catch (error) {
                                                                clientLogger.error("Error clearing panel sizes from localStorage:", error);
                                                            }
                                                            }, []);

                                                            return {
                                                                mainPanelSize,
                                                                sidebarPanelSize,
                                                                isFloatingMode,
                                                                floatingThreshold,
                                                                setMainPanelSize: setMainPanelSizePersistent,
                                                                setSidebarPanelSize: setSidebarPanelSizePersistent,
                                                                setFloatingThreshold: setFloatingThresholdPersistent,
                                                                toggleFloatingMode,
                                                                resetPanelSizes,
                                                                };

}

} // namespace elizaos
