#include "components/agent-creator.hpp"
#include "components/agent-log-viewer.hpp"
#include "components/app-sidebar.hpp"
#include "components/connection-error-banner.hpp"
#include "components/env-settings.hpp"
#include "components/onboarding-tour.hpp"
#include "components/ui/button.hpp"
#include "components/ui/sheet.hpp"
#include "components/ui/toaster.hpp"
#include "components/ui/tooltip.hpp"
#include "context/AuthContext.hpp"
#include "context/ConnectionContext.hpp"
#include "hooks/use-query-hooks.hpp"
#include "hooks/use-version.hpp"
#include "lib/api-client-config.hpp"
#include "routes/agent-settings.hpp"
#include "routes/chat.hpp"
#include "routes/createAgent.hpp"
#include "routes/group-new.hpp"
#include "routes/group.hpp"
#include "routes/home.hpp"
#include "routes/not-found.hpp"
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



// Create a query client with optimized settings
      // Default to no polling unless specifically configured
      // Make queries retry 3 times with exponential backoff
      // Refetch query on window focus
      // Enable refetch on reconnect
      // Fail queries that take too long
      // Default to 3 retries for mutations too

// Prefetch initial data with smarter error handling
    // Prefetch agents (real-time data so shorter stale time)
    // Don't throw, let the app continue loading with fallbacks

// Execute prefetch immediately

// Component containing the core application logic and routing
void AppContent();

// Main App component setting up providers
void App();


} // namespace elizaos
