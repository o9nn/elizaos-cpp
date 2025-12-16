#include "ChatImage.hpp"
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

// TODO: Rewrite as chat instead of generation

 // Import WebSocket utility

 // Import clsx for conditional classes

// --- API Base URL ---

// --- Constants for Chat ---
using ChatTier = (typeof CHAT_TIERS)[number];

// Helper functions for chat tiers

// --- Chat Types ---
// Chat Message Type (matches backend structure)
struct ChatMessage {
    std::string id;
    string; // User's public key author;
    std::optional<string | null; // Optional: Author's display name> displayName;
    std::optional<string | null; // Optional: Author's profile picture URL> profileImage;
    std::string tokenMint;
    std::string message;
    std::optional<string | null> parentId;
    ChatTier tier;
    std::optional<double> replyCount;
    std::string timestamp;
    std::optional<boolean; // Flag for optimistically added messages> isOptimistic;
    std::optional<string; // Added media field> media;
};

// API Response Types for Chat
struct EligibleTiersResponse {
    bool success;
    std::optional<std::vector<ChatTier>> tiers;
    std::optional<double> balance;
    std::optional<std::string> error;
};

struct GetMessagesResponse {
    bool success;
    std::optional<std::vector<ChatMessage>> messages;
    std::optional<std::string> error;
};

struct PostMessageResponse {
    bool success;
    std::optional<ChatMessage> message;
    std::optional<std::string> error;
};


} // namespace elizaos
