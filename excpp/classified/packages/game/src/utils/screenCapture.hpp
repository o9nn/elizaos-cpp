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
;

  try {
    // Check for navigator.mediaDevices
    if (!navigator.mediaDevices) {
      capabilities.error = 'navigator.mediaDevices not available';
      return capabilities;
    }

    // Check for getDisplayMedia (screen capture)
    if ('getDisplayMedia' in navigator.mediaDevices) {
      capabilities.displayMedia = true;
    } else {
      capabilities.error = 'getDisplayMedia not supported';
    }

    // Check for getUserMedia (camera/mic)
    if ('getUserMedia' in navigator.mediaDevices) {
      capabilities.getUserMedia = true;
    }

    capabilities.available = capabilities.displayMedia;
  } catch (error) {
    capabilities.error = `Error checking capabilities: ${error}`;
  }

  return capabilities;
}

/**
 * Test screen capture functionality
 */
std::future<bool> testScreenCapture();

  try {
    // Try to get a stream (user will need to approve)
    const stream = await navigator.mediaDevices.getDisplayMedia({
      video: { frameRate: { ideal: 1, max: 1 } }, // Low frame rate for test
      audio: false,
    });

    // Success - stop the stream immediately
    stream.getTracks().forEach((track) => track.stop());
    console.log('✅ Screen capture test successful');
    return true;
  } catch (error: any) {
    if (error.name === 'NotAllowedError') {
      console.log('ℹ️ Screen capture test cancelled by user');
      return true; // API is available, user just cancelled
    }
    console.error('❌ Screen capture test failed:', error);
    return false;
  }
}

/**
 * Get user-friendly error message for screen capture errors
 */

  if (error.name === 'NotFoundError') {
    return 'No screen sources available for sharing.';
  }
  if (error.name === 'NotReadableError') {
    return 'Screen source is currently unavailable. It may be in use by another application.';
  }
  if (error.name === 'OverconstrainedError') {
    return 'Screen sharing constraints cannot be satisfied.';
  }
  if (error.name === 'TypeError') {
    return 'Invalid screen sharing configuration.';
  }
  return `Screen sharing error: ${error.message || error.name || 'Unknown error'}`;
}

} // namespace elizaos
