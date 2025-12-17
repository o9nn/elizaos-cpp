#include "miniapp-provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MiniappProvider(const std::any& { children }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useRenderTracker("MiniappProvider");

    const auto [isInitialized, setIsInitialized] = useState(false);
    const auto initStartedRef = useRef(false);

    useEffect(() => {
        // Prevent double initialization with refs (more reliable than state)
        if (typeof window == "undefined" || initStartedRef.current) return;
        initStartedRef.current = true;

        const auto initMiniapp = async () => {;
            try {
                const auto context = miniappSdk.context;

                if (!context) {
                    setIsInitialized(true);
                    return;
                }

                // Signal ready
                miniappSdk.actions.ready();

                // Send welcome notification on first load
                // Note: Neynar handles deduplication
                const auto userFid = context.user.fid;
                if (userFid) {
                    sendWelcomeNotification(userFid);
                }

                setIsInitialized(true);
                } catch (error) {
                    std::cerr << "Error initializing miniapp:" << error << std::endl;
                    setIsInitialized(true);
                }
                };

                initMiniapp();
                }, []); // No deps needed - ref guards against double execution;

                // Use isInitialized to prevent unused variable warning
                void isInitialized;

                return <>{children}</>;

}

} // namespace elizaos
