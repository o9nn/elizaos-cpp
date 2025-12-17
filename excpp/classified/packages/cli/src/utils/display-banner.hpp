#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "bun-exec.hpp"
#include "user-environment.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Export function to display banner and version

// Helper function to check if running from node_modules
bool isRunningFromNodeModules();

// Function to get the package version
// --- Utility: Get local CLI version from package.json ---
std::string getVersion();

// --- Utility: Get install tag based on CLI version ---
std::string getCliInstallTag();

// --- Utility: Check if terminal supports UTF-8 ---
void isUtf8Locale();

// Cache for version check to avoid multiple network calls in same session

// --- Utility: Get latest CLI version with caching ---

// --- Utility: Display compact, professional update notification ---
void showUpdateNotification(const std::string& currentVersion, const std::string& latestVersion);

// --- Utility: Global update check that can be called from anywhere ---
std::future<bool> checkAndShowUpdateNotification(const std::string& currentVersion);

// --- Main: Display banner and version, then check for updates ---
std::future<void> displayBanner(boolean = false skipUpdateCheck);

} // namespace elizaos
