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



struct ChatProps {
    std::optional<std::string> roomId;
    std::optional<Token> token;
    std::optional<std::optional<TokenMarketData>> marketData;
};

// --- Consolidated Chat State ---
struct ChatState {
    std::vector<ChatMessage> messages;
    std::string input;
    bool inputDisabled;
    std::string roomId;
    bool isLoadingHistory;
    bool isAgentThinking;
    std::string entityId;
    bool showConnectOverlay;
    std::optional<OTCQuote> currentQuote;
    bool showAcceptModal;
    bool isOfferGlowing;
    bool showClearChatModal;
};

using ChatAction = std::variant<, { type: "SET_MESSAGES">; payload: ChatMessage[] }

ChatState chatReducer(ChatState state, ChatAction action);

// Raw message format from API
struct RawRoomMessage {
    std::optional<std::string> id;
    std::optional<std::string> entityId;
    std::optional<std::string> agentId;
    std::optional<std::variant<double, std::string>> createdAt;
    std::optional<std::string> text;
};

// --- Helper: Parse room message into ChatMessage format ---
std::optional<ChatMessage> parseRoomMessage(RawRoomMessage msg, const std::string& roomId);

  // --- Consolidated State ---

  // --- Refs ---

  // Initialize user from connected wallet; gate chat when disconnected

      // Use initialRoomId if provided, else stored room, else null (will create new)

  // Function to create a new room

    // Persist room per-wallet

  // Load room data - only when roomId or entityId changes, NOT on messages change

        // Format messages using helper function

        // Sort by timestamp

        // Update last message timestamp

  // Store messages ref for polling to avoid stale closure issues

  // Poll for new messages when agent is thinking

    // Poll every 2 seconds for new messages (faster polling has diminishing returns)

          // Use ref to get current messages without triggering effect restart

          // Merge with new messages and dedupe by server ID

          // Update last message timestamp

          // Check if we received an agent message

    // Stop polling after 30 seconds

  // Send message function - accepts optional targetRoomId to handle newly created rooms

      // Add user message to UI immediately with a client-generated ID

        // Retry once on transient server errors

      // Prefer server timestamp to avoid client/server clock skew missing agent replies
        // Set to just before our message so we catch both our message and agent's response
        // Fallback: use current time minus a buffer

  // Handle form submit

      // Ensure user is connected and room exists before sending

      // Pass activeRoomId explicitly in case it was just created (state not yet updated)

      // Refocus the textarea after sending

  // Handle creating a new room when there isn't one
      // Automatically create a room for this wallet when connected

  // Auto-scroll to bottom on new messages

  // Extract current quote from messages

    // Find the latest quote in messages

        // Only update if quote actually changed
          // Trigger glow effect only if there was a previous quote

          // Update the ref and state

    // Validate quote has required fields
      // Don't show alert, just log - user should request a new quote via chat

    // Clear local storage for this wallet

    // Create a new room

    // Clear messages and reset state

    // DO NOT close the modal - let it show the success state and handle its own redirect
    // The modal will redirect to /deal/[id] page after 2 seconds

    // Reset chat and create new room in the background

    // Clear local storage for this wallet

    // Create a new room
      // Still clear the old state even if new room creation failed

    // Clear messages and reset state - this prepares a fresh chat for when user returns

  // Unified connect handler - uses connectWallet if already authenticated, login if not

  // Switch chain - just changes active family if already connected, otherwise prompts connect

      // If not connected to that chain, prompt connect/login

  // Memoized setters for child components

  // Use the quote passed from parent (extracted from messages)

  // Determine if user needs to switch chains


} // namespace elizaos
