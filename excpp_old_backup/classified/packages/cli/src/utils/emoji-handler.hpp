#include "elizaos/core.hpp"
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
 * Cross-platform emoji handler that provides fallbacks for terminals that don't support emojis
 */

struct EmojiConfig {
    bool enabled;
    bool forceDisable;
};

// Global configuration

/**
 * Emoji definitions with fallbacks
 */
  // Status indicators

  // Actions

  // Objects/Tools

  // Platforms

  // Arrows and pointers

using EmojiKey = keyof typeof EMOJIS;

/**
 * Detect if the current terminal/environment supports emojis
 */
bool detectEmojiSupport();

/**
 * Get an emoji with appropriate fallback
 */
std::string getEmoji(EmojiKey key);

/**
 * Configure emoji behavior
 */
void configureEmojis(const std::optional<EmojiConfig>& newConfig);

/**
 * Get current emoji configuration
 */
EmojiConfig getEmojiConfig();

/**
 * Check if emojis are currently enabled and supported
 */
bool areEmojisEnabled();

/**
 * Format a message with an emoji prefix
 */
std::string withEmoji(EmojiKey key, const std::string& message, boolean = true spacing);

/**
 * Utility functions for common patterns
 */

/**
 * Auto-detect and initialize emoji support on module load
 */
void initializeEmojiSupport();

// Initialize on import

} // namespace elizaos
