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

;

/**
 * Cross-platform emoji handler that provides fallbacks for terminals that don't support emojis
 */

struct EmojiConfig {
    bool enabled;
    bool forceDisable;
};


// Global configuration
let config: EmojiConfig = {
  enabled: true,
  forceDisable: false,
};

/**
 * Emoji definitions with fallbacks
 */
const EMOJIS = {
  // Status indicators
  success: { emoji: '✅', fallback: '[OK]' },
  error: { emoji: '❌', fallback: '[ERROR]' },
  warning: { emoji: '⚠️', fallback: '[WARNING]' },
  info: { emoji: 'ℹ️', fallback: '[INFO]' },

  // Actions
  rocket: { emoji: '🚀', fallback: '>>' },
  sparkles: { emoji: '✨', fallback: '*' },
  party: { emoji: '🎉', fallback: '[DONE]' },

  // Objects/Tools
  package: { emoji: '📦', fallback: '[PKG]' },
  link: { emoji: '🔗', fallback: '[LINK]' },
  lightbulb: { emoji: '💡', fallback: '[TIP]' },
  clipboard: { emoji: '📋', fallback: '[LIST]' },

  // Platforms
  penguin: { emoji: '🐧', fallback: '[LINUX]' },
  globe: { emoji: '🌐', fallback: '[WEB]' },

  // Arrows and pointers
  rightArrow: { emoji: '→', fallback: '->' },
  bullet: { emoji: '•', fallback: '*' },
} as const;

using EmojiKey = keyof typeof EMOJIS;

/**
 * Detect if the current terminal/environment supports emojis
 */


  // Check environment variables that indicate emoji support
  const term = process.env.TERM || '';
  const termProgram = process.env.TERM_PROGRAM || '';
  const colorTerm = process.env.COLORTERM;
  const ciEnv = process.env.CI;

  // CI environments often don't render emojis well
  if (ciEnv === 'true' || process.env.GITHUB_ACTIONS) {
    return false;
  }

  // Windows Command Prompt and older terminals
  if (process.platform === 'win32') {
    // Windows Terminal, VS Code terminal, and newer terminals support emojis
    if (
      termProgram === 'vscode' ||
      process.env.WT_SESSION ||
      process.env.WT_PROFILE_ID ||
      termProgram === 'Windows Terminal'
    ) {
      return true;
    }

    // PowerShell 7+ generally supports emojis
    if (process.env.PSModulePath && process.env.POWERSHELL_TELEMETRY_OPTOUT !== undefined) {
      return true;
    }

    // Default to false for Windows unless we're sure
    return false;
  }

  // macOS and Linux
  if (process.platform === 'darwin' || process.platform === 'linux') {
    // Modern terminals support emojis
    if (
      termProgram === 'vscode' ||
      termProgram === 'iTerm.app' ||
      termProgram === 'Apple_Terminal' ||
      term.includes('xterm') ||
      term.includes('screen') ||
      colorTerm
    ) {
      return true;
    }
  }

  // Check for specific terminal capabilities
  if (term.includes('256color') || term.includes('truecolor')) {
    return true;
  }

  // Conservative default - assume no emoji support unless we're confident
  return false;
}

/**
 * Get an emoji with appropriate fallback
 */
`);
    return '';
  }

  return config.enabled && detectEmojiSupport() ? emojiDef.emoji : emojiDef.fallback;
}

/**
 * Configure emoji behavior
 */
;
}

/**
 * Get current emoji configuration
 */
;
}

/**
 * Check if emojis are currently enabled and supported
 */


/**
 * Format a message with an emoji prefix
 */
${space}${message}`;
}

/**
 * Utility functions for common patterns
 */
const emoji = {
  success: (msg: string) => withEmoji('success', msg),
  error: (msg: string) => withEmoji('error', msg),
  warning: (msg: string) => withEmoji('warning', msg),
  info: (msg: string) => withEmoji('info', msg),
  rocket: (msg: string) => withEmoji('rocket', msg),
  package: (msg: string) => withEmoji('package', msg),
  link: (msg: string) => withEmoji('link', msg),
  tip: (msg: string) => withEmoji('lightbulb', msg),
  list: (msg: string) => withEmoji('clipboard', msg),
  penguin: (msg: string) => withEmoji('penguin', msg),
  bullet: (msg: string) => withEmoji('bullet', msg),
};

/**
 * Auto-detect and initialize emoji support on module load
 */
 (platform: ${process.platform}, term: ${process.env.TERM || 'unknown'})`
    );
  }
}

// Initialize on import
initializeEmojiSupport();

} // namespace elizaos
