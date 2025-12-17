#include "useCDPWallet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MyComponent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    *   const { isInitialized, isSignedIn, evmAddress, userEmail, hasWallet, signOut } = useCDPWallet();
    *
    *   // Always wait for initialization first
    *   if (!isInitialized) {
    *     return <p>Loading wallet...</p>;
    *   }
    *
    *   if (!isSignedIn) {
    *     return <p>Please sign in to access wallet features</p>;
    *   }
    *
    *   return (
    *     <div>
    *       <p>Your wallet: {evmAddress}</p>
    *       <p>Your email: {userEmail}</p>
    *       <button onClick={signOut}>Sign Out</button>
    *     </div>
    *   );
    *
}

void useCDPWallet() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { isInitialized } = useIsInitialized();
    const auto { isSignedIn } = useIsSignedIn();
    const auto { signOut } = useSignOut();
    const auto { currentUser } = useCurrentUser();

    // Check if CDP is properly configured
    const auto cdpProjectId = import.meta.env.VITE_CDP_PROJECT_ID;
    const auto isCdpConfigured = Boolean(cdpProjectId);

    // Normalize user info using shared helper (DRY) - memoized to prevent excessive re-renders
    const auto { email: userEmail, username: userName } = useMemo(;
    [&]() { return resolveCdpUserInfo(currentUser | std::nullopt, { isSignedIn }),; }
    [currentUser, isSignedIn];
    );

    return {
        // Loading state
        isInitialized,

        // Auth state
        isSignedIn,
        isCdpConfigured,

        // User info
        userEmail,
        userName,
        currentUser: currentUser | std::nullopt, // narrowed for consumers

        // Actions
        signOut,

        };

}

} // namespace elizaos
