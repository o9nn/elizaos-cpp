#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "use-toast.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Direct error handling without bridge layer

// Create ElizaClient instance for direct API calls

/**
 * Represents content with additional user information.
 * @typedef {Object} ContentWithUser
 * @property {string} name - The name of the user.
 * @property {number} createdAt - The timestamp when the content was created.
 * @property {boolean} [isLoading] - Optional flag indicating if the content is currently loading.
 * @property {string} [worldId] - Optional ID of the world associated with the content.
 * @property {string} [id] - Optional ID field.
 */
using ContentWithUser = Content & {

// AgentLog type from the API
using AgentLog = {

// Constants for stale times

// Network Information API interface
/**
 * Interface for representing network information.
 *
 * @property {("slow-2g" | "2g" | "3g" | "4g" | "unknown")} effectiveType - The effective network type.
 * @property {boolean} saveData - Indicates if data saver mode is enabled.
 * @property {unknown} [key] - Additional properties with unknown value types.
 */

struct NetworkInformation {
    std::variant<'slow-2g', '2g', '3g', '4g', 'unknown'> effectiveType;
    bool saveData;
};

// Network status detection for smart polling
/**
 * A custom React hook that returns the network status information.
 * Utilizes the Network Information API if available.
 * @returns {{
 *  isOffline: boolean,
 *  effectiveType: string,
 *  saveData: boolean
 * }} The network status information including whether the user is offline, the effective connection type, and if data-saving mode is enabled.
 */
  // Get navigator.connection if available (Network Information API)

  // Return the effective connection type or a default value

// Hook for fetching agents with smart polling
/**
 * Fetches a list of agents from the server with polling and network-aware intervals.
 *
 * @param options - Optional configuration to override default query behavior.
 * @returns A React Query object containing the agents data and query state.
 *
 * @remark Polling frequency adapts to network conditions, using less frequent polling when offline or on slow connections.
 */
void useAgents(auto options = {});

// Hook for fetching a specific agent with smart polling
/**
 * Custom hook to fetch agent data based on the provided agentId.
 * @param {UUID | undefined | null} agentId - The ID of the agent to fetch data for.
 * @param {Object} options - Additional options to configure the query.
 * @returns {QueryResult} The result of the query containing agent data.
 */
void useAgent(const std::optional<UUID>& agentId, auto options = {});

// Hook for starting an agent with optimistic updates
/**
 * Custom hook to start an agent by calling the API with the provided agent ID.
 *
 * @returns {MutationFunction<UUID, unknown>} The useMutation hook for starting an agent.
 */
void useStartAgent();

// Hook for stopping an agent with optimistic updates
/**
 * Custom hook to stop an agent by calling the API and updating the UI optimistically.
 *
 * @returns {UseMutationResult} - Object containing the mutation function and its handlers.
 */
void useStopAgent();

// Type for UI message list items
using UiMessage = Content & {
  // attachments and other Content props are inherited

/**
 * Custom hook to manage fetching and loading messages for a specific channel.
 * @param {UUID | undefined} channelId - The GLOBAL ID of the channel.
 * @returns {{...
 */

  // Using a more manual approach for pagination with getChannelMessages

          // If direct parse fails, try moment (if available and robust)
          // For now, log a warning if it's an unparsable string not handled by Date.parse
          // As a fallback, could try new Date(sm.createdAt).getTime(), but Date.parse is usually sufficient
          // Defaulting to Date.now() if unparsable to avoid NaN
        // If it's not a number or string, but exists (e.g. could be a Date object from some contexts)
        // Attempt to convert. This is less likely if types are strict from server.

      // eslint-disable-next-line react-hooks/exhaustive-deps

    // Initial fetch when channelId changes or becomes available
    // eslint-disable-next-line react-hooks/exhaustive-deps

  // Add method to manually add/update messages from external sources (e.g., WebSocket)
      // Check if message already exists

        // Update existing message
        // Add new message

  // Add method to update a message by ID

  // Add method to remove a message by ID

  // Add method to clear all messages

  // This hook now manages its own state for messages
  // To integrate with React Query for caching of initial load or background updates:
  // One could use useInfiniteQuery, but given the manual state management already here for append/prepend,
  // this simpler useState + manual fetch approach is retained from the original structure of useMessages.
  // For full React Query benefits, `useInfiniteQuery` would be the way to go.

void useGroupChannelMessages(const std::optional<UUID>& channelId, std::optional<UUID> initialServerId);

// Hook for fetching agent actions
/**
 * Custom hook to fetch agent actions for a specific agent and room.
 * @param {UUID} agentId - The ID of the agent.
 * @param {UUID} roomId - The ID of the room.
 * @param {string[]} excludeTypes - Optional array of types to exclude from results.
 * @returns {QueryResult} The result of the query containing agent actions.
 */
void useAgentActions(UUID agentId, std::optional<UUID> roomId, std::optional<std::vector<std::string>> excludeTypes);

/**
 * Hook to delete an agent log/action.
 * @returns {UseMutationResult} - Object containing the mutation function and its handlers.
 */
void useDeleteLog();

/**
 * Fetches memories for a specific agent, optionally filtered by channel
 */

/**
 * Provides a mutation hook to delete a specific memory entry for an agent.
 *
 * On success, invalidates related agent and room memory queries to ensure data consistency.
 */
void useDeleteMemory();

/**
 * Hook for deleting all memories associated with a specific agent in a given room.
 *
 * @returns A mutation object for triggering the deletion and tracking its state.
 */
void useDeleteAllMemories();

/**
 * Updates a specific memory entry for an agent.
 *
 * Triggers cache invalidation for related agent and room memories, as well as messages, to ensure data consistency. Displays a toast notification on success or error.
 *
 * @returns A mutation object for updating an agent's memory entry.
 */
void useUpdateMemory();

void useDeleteGroupMemory();

void useClearGroupChat();

// REMOVED: useRooms - Client should use channels, not rooms
// Rooms are an agent-only abstraction

// Hook for fetching agent panels (public GET routes)
/**
 * Custom hook to fetch public GET routes (panels) for a specific agent.
 * @param {UUID | undefined | null} agentId - The ID of the agent.
 * @param {object} options - Optional TanStack Query options.
 * @returns {QueryResult} The result of the query containing agent panels.
 */
using AgentPanel = {

void useAgentPanels(const std::optional<UUID>& agentId, auto options = {});

/**
 * Custom hook that combines useAgents with individual useAgent calls for detailed data
 * @returns {AgentsWithDetailsResult} Combined query results with both list and detailed data
 */
struct AgentsWithDetailsResult {
    std::vector<Agent> agents;
    bool isLoading;
    bool isError;
    unknown error;
};

/**
 * Fetches a list of agents with detailed information for each agent in parallel.
 *
 * Combines the agent list from {@link useAgents} with individual agent detail queries using `useQueries`, aggregating loading and error states. Polling intervals adapt to network conditions.
 *
 * @returns An object containing detailed agent data, loading and error states, and any encountered error.
 */
AgentsWithDetailsResult useAgentsWithDetails();

// --- Hooks for Admin/Debug (Agent-Perspective Data) ---
void useAgentInternalActions(const std::optional<UUID>& agentId, std::optional<std::optional<UUID>> agentPerspectiveRoomId);

void useDeleteAgentInternalLog();

void useAgentInternalMemories(const std::optional<UUID>& agentId, const std::optional<UUID>& agentPerspectiveRoomId, string = 'messages' tableName, auto includeEmbedding = false);

void useDeleteAgentInternalMemory();

void useDeleteAllAgentInternalMemories();

void useUpdateAgentInternalMemory();

// --- Hooks for Servers and Channels (GUI Navigation) ---
void useServers(auto options = {});

void useChannels(UUID serverId, auto options = {});

void useChannelDetails(UUID channelId, auto options = {});

void useChannelParticipants(UUID channelId, auto options = {});

void useDeleteChannelMessage();

void useClearChannelMessages();

void useDeleteChannel();

} // namespace elizaos
