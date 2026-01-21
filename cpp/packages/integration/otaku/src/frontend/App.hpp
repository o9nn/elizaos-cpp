#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "components/auth/SignInModal.hpp"
#include "components/chat/chat-interface.hpp"
#include "components/dashboard/account/page.hpp"
#include "components/dashboard/cdp-wallet-card.hpp"
#include "components/dashboard/leaderboard/page.hpp"
#include "components/dashboard/mobile-header.hpp"
#include "components/dashboard/notifications/collapsible-notifications.hpp"
#include "components/dashboard/sidebar.hpp"
#include "components/dashboard/widget.hpp"
#include "components/ui/sidebar.hpp"
#include "contexts/LoadingPanelContext.hpp"
#include "contexts/ModalContext.hpp"
#include "elizaos/core.hpp"
#include "hooks/useCDPWallet.hpp"
#include "lib/elizaClient.hpp"
#include "lib/socketManager.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Available default avatars for deterministic randomization

// Check if avatar is the default krimson avatar (early users have this)

// Deterministic avatar selection based on userId (same user always gets same avatar)

/**
 * Authenticate with backend and get JWT token
 * Uses CDP's userId as the primary identifier
 * 
 * @param email User's email from CDP authentication
 * @param username User's display name from CDP
 * @param currentUser CDP currentUser object (to extract userId)
 */
    
    // Extract CDP userId

    // Login with backend - send email, username, and CDP userId
    
    // Store token in localStorage
    
    // Set token for all API calls

struct Channel {
    std::string id;
    std::string name;
    std::optional<double> createdAt;
};

void App();

// Inner component that has access to useSidebar
void AppContent(auto userId, auto connected, auto channels, auto activeChannelId, auto isCreatingChannel, auto isNewChatMode, auto currentView, auto userProfile, auto totalBalance, auto isLoadingChannels, auto walletRef, auto handleNewChat, auto handleChannelSelect, auto handleBalanceChange, auto setChannels, auto setActiveChannelId, auto setIsNewChatMode, auto updateUserProfile, auto signOut, auto isSignedIn, auto agentId, auto navigate);

// Wrap App with CDP Provider (if configured) and LoadingPanelProvider

} // namespace elizaos
