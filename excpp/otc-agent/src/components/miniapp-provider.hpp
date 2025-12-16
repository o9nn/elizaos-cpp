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

"use client";

;
;
;
;

: { children: React.ReactNode }) {
  useRenderTracker("MiniappProvider");

  const [isInitialized, setIsInitialized] = useState(false);
  const initStartedRef = useRef(false);

  useEffect(() => {
    // Prevent double initialization with refs (more reliable than state)
    if (typeof window === "undefined" || initStartedRef.current) return;
    initStartedRef.current = true;

    const initMiniapp = async () => {
      try {
        const context = await miniappSdk.context;

        if (!context) {
          setIsInitialized(true);
          return;
        }

        // Signal ready
        await miniappSdk.actions.ready();

        // Send welcome notification on first load
        // Note: Neynar handles deduplication
        const userFid = context.user?.fid;
        if (userFid) {
          await sendWelcomeNotification(userFid);
        }

        setIsInitialized(true);
      } catch (error) {
        console.error("Error initializing miniapp:", error);
        setIsInitialized(true);
      }
    };

    initMiniapp();
  }, []); // No deps needed - ref guards against double execution

  // Use isInitialized to prevent unused variable warning
  void isInitialized;

  return <>{children}</>;
}

} // namespace elizaos
