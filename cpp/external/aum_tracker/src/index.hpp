#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "components/ModernDashboard.hpp"
#include "components/ModernLayout.hpp"
#include "components/TokenDetail.hpp"
#include "components/WalletDetail.hpp"
#include "routes/api.hpp"
#include "scripts/prefetch.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Middleware

// JSX renderer

// Serve static files

// API routes

// Dashboard page

// Wallet detail page

// Token detail page

// Health check

// 404 handler

// Error handler

// Auto-prefetch on startup (full refresh)
std::future<void> startupPrefetch();

// Start prefetch after a longer delay to allow server to fully initialize


} // namespace elizaos
