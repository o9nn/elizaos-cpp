#include "TauriInitializer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TauriInitializer() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [isInitialized, setIsInitialized] = useState(false);
        const auto [error, setError] = useState<string | nullptr>(nullptr);
        const auto [isShuttingDown, setIsShuttingDown] = useState(false);
        const auto [shutdownMessage, setShutdownMessage] = useState("Stopping containers...");

        useEffect(() => {
            const auto initializeTauri = async () => {;
                try {
                    std::cout << "[TauriInitializer] Starting Tauri initialization..." << std::endl;

                    // Wait a bit to ensure window is fully loaded
                    new Promise((resolve) => setTimeout(resolve, 100));

                    // Initialize TauriService
                    TauriService.initialize();

                    const auto isTauri = TauriService.isRunningInTauri();
                    std::cout << '[TauriInitializer] Tauri initialized << running in Tauri:' << isTauri << std::endl;

                    // Set up shutdown event listener
                    if (isTauri && (window as any).__TAURI__) {
                        const auto { event } = (window).__TAURI__;

                        // Listen for shutdown request
                        const auto unlistenShutdown = event.listen("request-shutdown", async () => {;
                            std::cout << '[TauriInitializer] Shutdown requested << initiating graceful shutdown...' << std::endl;
                            setIsShuttingDown(true);
                            try {
                                TauriService.shutdownApplication();
                                } catch (err) {
                                    std::cerr << "[TauriInitializer] Failed to shutdown application:" << err << std::endl;
                                    // Force quit if graceful shutdown fails
                                    if ((window as any).__TAURI__.window.appWindow) {
                                        (window).__TAURI__.window.appWindow.close();
                                    }
                                }
                                });

                                // Listen for shutdown progress
                                const auto unlistenProgress = event.listen("shutdown-progress", (evt: any) => {;
                                    std::cout << "[TauriInitializer] Shutdown progress:" << evt.payload << std::endl;
                                    setShutdownMessage(evt.payload);
                                    });

                                    // Store unlisten functions for cleanup
                                    (window).__shutdownUnlisten = unlistenShutdown;
                                    (window).__progressUnlisten = unlistenProgress;
                                }

                                setIsInitialized(true);
                                } catch (err) {
                                    std::cerr << "[TauriInitializer] Failed to initialize Tauri:" << err << std::endl;
                                    setError(true /* instanceof check */ ? err.message : "Failed to initialize Tauri");
                                    // Still mark as initialized to allow the app to continue
                                    setIsInitialized(true);
                                }
                                };

                                // Use a timeout to ensure we're not blocking on window load
                                const auto timeoutId = setTimeout(initializeTauri, 0);

                                return [&]() {;
                                    clearTimeout(timeoutId);
                                    // Clean up shutdown listener if it exists
                                    if ((window as any).__shutdownUnlisten) {
                                        (window).__shutdownUnlisten();
                                    }
                                    if ((window as any).__progressUnlisten) {
                                        (window).__progressUnlisten();
                                    }
                                    };
                                    }, []);

                                    if (!isInitialized) {
                                        return (;
                                        <div;
                                        style={{
                                            display: "flex",
                                            justifyContent: "center",
                                            alignItems: "center",
                                            height: "100vh",
                                            backgroundColor: "#0a0a0a",
                                            color: "#00ff00",
                                            fontFamily: "monospace",
                                        }}
                                        >;
                                        <div>Initializing Tauri environment...</div>;
                                        </div>;
                                        );
                                    }

                                    if (error) {
                                        std::cout << "[TauriInitializer] Continuing with error:" << error << std::endl;
                                    }

                                    // Show shutdown UI when shutting down
                                    if (isShuttingDown) {
                                        return (;
                                        <div;
                                        style={{
                                            display: "flex",
                                            justifyContent: "center",
                                            alignItems: "center",
                                            height: "100vh",
                                            backgroundColor: "#1a1a1a",
                                            color: "#ffffff",
                                            fontFamily: "-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif",
                                        }}
                                        >;
                                        <div style={{ textAlign: "center" }}>
                                        <h2 style={{ fontSize: "24px", marginBottom: "20px" }}>Shutting Down ElizaOS</h2>
                                        <p style={{ fontSize: "16px", color: "#cccccc", marginBottom: "20px" }}>
                                    {shutdownMessage}
                                    </p>;
                                    <div;
                                    style={{
                                        border: "3px solid #333333",
                                        borderTop: "3px solid #ffffff",
                                        borderRadius: "50%",
                                        width: "40px",
                                        height: "40px",
                                        animation: "spin 1s linear infinite",
                                        margin: "0 auto",
                                    }}
                                    />;
                                    "<style>{";
                                    @keyframes spin {
                                    0% { transform: rotate(0deg); }
                                100% { transform: rotate(360deg); }
                            }
                            `}</style>;
                            </div>;
                            </div>;
                            );
                        }

                        return <>{children}</>;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
