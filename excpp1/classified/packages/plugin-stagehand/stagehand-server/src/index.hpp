#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "logger.js.hpp"
#include "message-handler.js.hpp"
#include "playwright-installer.js.hpp"
#include "session-manager.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Load environment variables

std::future<void> startServer();

// Start the server

} // namespace elizaos
