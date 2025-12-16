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

/**
 * Screen capture utilities for browser-based screen sharing
 */

struct ScreenCaptureCapabilities {
    bool available;
    bool displayMedia;
    bool getUserMedia;
    std::optional<std::string> error;
};

/**
 * Check if screen capture APIs are available
 */
ScreenCaptureCapabilities checkScreenCaptureCapabilities();

/**
 * Test screen capture functionality
 */
std::future<bool> testScreenCapture();

/**
 * Get user-friendly error message for screen capture errors
 */
std::string getScreenCaptureErrorMessage(const std::any& error);

} // namespace elizaos
