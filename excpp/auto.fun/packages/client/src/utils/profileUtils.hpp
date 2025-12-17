#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "api.hpp"
#include "env.hpp"
#include "program.hpp"
#include "swapUtils.hpp"
#include "types/profileTypes.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// --- Types for User Profile Data ---
struct UserProfileData {
    std::string id;
    std::string address;
    std::string displayName;
    std::string profilePictureUrl;
    double points;
    double rewardPoints;
    std::string createdAt;
    double suspended;
};

struct ProfileApiResponse {
    UserProfileData user;
    std::vector<std::any> transactions;
    std::vector<ProfileToken> tokensCreated;
};

// --- End Types ---

// TODO: update after mainnet launch

using Account = {

  // Skip key, update authority, mint, and name length prefix

  // Read name

  // Read symbol length and symbol

  // Read uri length and uri

using MetadataAccount = AccountInfo<Buffer>;

          // Skip if URI is not HTTPS

    // Get all tokens with bonding curve info

    // Get metadata for all owned tokens

      // const mint = t.mint; // Extract mint property
      // parse the mint

      // find the account with this mint (if any)

        // safe‐cast the amount or fall back to zero

// --- New Hook: useUserProfile ---
// Fetches public user data (info, created tokens) for a given address

        // Log the status text before throwing

// --- End useUserProfile Hook ---

  // Use our new hook to fetch user profile data

    // Get tokensCreated from the profileData

    // always update the state even if one fails

    // update profile automatically when the user's wallet account changes

  // Combine loading states
  // Set error if either operation fails

// Function to update user profile

// Function to upload a new profile picture

  // Basic client-side validation (optional, as backend also validates)
    // 5MB limit

    // Content-Type is set automatically by browser for FormData

  // Return the updated user data from the response

// Function to trigger profile picture generation (placeholder)


} // namespace elizaos
