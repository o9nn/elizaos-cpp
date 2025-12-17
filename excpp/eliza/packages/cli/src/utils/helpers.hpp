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
 * Display character
 */
void displayAgent(const std::optional<Agent>& data, auto title);

/**
 * Formats a conversation into a string
 */
std::string formatConversation(const std::vector<MessageExample>& conversation);

/**
 * Displays a section with a title and list of items
 */
void displaySection(const std::string& title, const std::vector<std::string>& items);

/**
 * Logs a header inside a rectangular frame with extra padding.
 * @param {string} title - The header text to display.
 */
void logHeader(const std::string& title);

} // namespace elizaos
