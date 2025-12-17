#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Example message for demonstration
 */
struct MessageExample {
    std::string name;
    Content content;
};

using TemplateType = std::function<void()>;

/**
 * Configuration for an agent's character, defining its personality, knowledge, and capabilities.
 * This is a central piece of an agent's definition, used by the `AgentRuntime` to initialize and operate the agent.
 * It includes:
 * - `id`: Optional unique identifier for the character.
 * - `name`, `username`: Identifying names for the character.
 * - `system`: A system prompt that guides the agent's overall behavior.
 * - `templates`: A map of prompt templates for various situations (e.g., message generation, summarization).
 * - `bio`: A textual biography or description of the character.
 * - `messageExamples`, `postExamples`: Examples of how the character communicates.
 * - `topics`, `adjectives`: Keywords describing the character's knowledge areas and traits.
 * - `knowledge`: Paths to knowledge files or directories to be loaded into the agent's memory.
 * - `plugins`: A list of plugin names to be loaded for this character.
 * - `settings`, `secrets`: Configuration key-value pairs, with secrets being handled more securely.
 * - `style`: Guidelines for the character's writing style in different contexts (chat, post).
 */
struct Character {
    std::optional<UUID> id;
    std::string name;
    std::optional<std::string> username;
    std::optional<std::string> system;
    std::variant<std::string, std::vector<std::string>> bio;
    std::optional<std::vector<std::vector<MessageExample>>> messageExamples;
    std::optional<std::vector<std::string>> postExamples;
    std::optional<std::vector<std::string>> topics;
    std::optional<std::vector<std::string>> adjectives;
    std::optional<std::vector<std::string>> plugins;
    std::optional<std::vector<std::string>> all;
    std::optional<std::vector<std::string>> chat;
    std::optional<std::vector<std::string>> post;
};

enum AgentStatus {
}

/**
 * Represents an operational agent, extending the `Character` definition with runtime status and timestamps.
 * While `Character` defines the blueprint, `Agent` represents an instantiated and potentially running version.
 * It includes:
 * - `enabled`: A boolean indicating if the agent is currently active or disabled.
 * - `status`: The current operational status, typically `AgentStatus.ACTIVE` or `AgentStatus.INACTIVE`.
 * - `createdAt`, `updatedAt`: Timestamps for when the agent record was created and last updated in the database.
 * This interface is primarily used by the `IDatabaseAdapter` for agent management.
 */

} // namespace elizaos
