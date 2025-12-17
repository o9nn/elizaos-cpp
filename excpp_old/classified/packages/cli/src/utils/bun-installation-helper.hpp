#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "bun-exec.hpp"
#include "elizaos/core.hpp"
#include "emoji-handler.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct BunInstallationResult {
    bool installed;
    std::string message;
    std::optional<std::string> error;
};

std::future<BunInstallationResult> checkBunInstallation();

void displayBunInstallInstructions();

/**
 * Returns a compact installation tip for bun
 */
std::string displayBunInstallationTipCompact();

} // namespace elizaos
