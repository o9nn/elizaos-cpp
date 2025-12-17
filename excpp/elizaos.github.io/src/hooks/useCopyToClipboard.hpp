#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * POSIX-style fallback for browsers that still allow `execCommand("copy")`.
 * Creates a temporary textarea element, selects its content, and uses the legacy
 * execCommand API to copy text to the clipboard.
 *
 * @param text - The text to copy to the clipboard
 * @returns `true` if the copy operation succeeded, `false` otherwise
 */
bool fallbackCopy(const std::string& text);

/**
 * Custom hook for copying text to the clipboard with fallback support.
 * Uses the modern Clipboard API when available, falling back to execCommand for older browsers.
 *
 * @returns A tuple containing:
 *   - `copied`: Boolean indicating if text was recently copied (auto-resets after 2 seconds)
 *   - `copyToClipboard`: Async function that copies the provided text to clipboard
 */

} // namespace elizaos
