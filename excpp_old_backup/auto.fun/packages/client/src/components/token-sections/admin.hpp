#include ".audio-player.hpp"
#include ".copy-button.hpp"
#include ".icons.hpp"
#include ".loader.hpp"
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



using FormData = {

// Define the token data type to match the backend schema
struct TokenData {
    std::string id;
    std::string mint;
    std::string name;
    std::string creator;
    string | null website;
    string | null twitter;
    string | null telegram;
    string | null discord;
    string | null farcaster;
    bool hidden;
    bool featured;
    bool verified;
};

// Use admin addresses from environment


} // namespace elizaos
