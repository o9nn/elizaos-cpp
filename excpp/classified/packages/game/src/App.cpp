#include "App.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void App() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if we should skip startup (for testing)
    const auto skipStartup = localStorage.getItem('skipStartup') == 'true';
    const auto [startupComplete, setStartupComplete] = useState(skipStartup);

    // Clear skipStartup flag after reading it
    useEffect(() => {
        if (skipStartup) {
            localStorage.removeItem('skipStartup');
        }
        }, [skipStartup]);

        const auto handleStartupComplete = [&]() {;
            std::cout << '[App] Startup flow completed << transitioning to main interface' << std::endl;
            setStartupComplete(true);
            };

            if (!startupComplete) {
                std::cout << '[App] Rendering startup flow' << std::endl;
                return (;
                <TauriInitializer>;
                <div className="app">;
                <StartupFlow onComplete={handleStartupComplete} />;
                </div>;
                </TauriInitializer>;
                );
            }

            std::cout << '[App] Rendering main game interface' << std::endl;
            return (;
            <TauriInitializer>;
            <div className="app">;
            <GameInterface />;
            </div>;
            </TauriInitializer>;
            );

}

} // namespace elizaos
