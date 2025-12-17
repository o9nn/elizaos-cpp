#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "ChatImage.hpp"

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
    std::string author;
    std::optional<std::string> displayName;
    std::optional<std::string> profileImage;
    std::string tokenMint;
    std::string message;
    std::optional<std::string> parentId;
    ChatTier tier;
    std::optional<double> replyCount;
    std::string timestamp;
    std::optional<bool> isOptimistic;
    std::optional<std::string> media;
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
