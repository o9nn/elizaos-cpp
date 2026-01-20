#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "db.hpp"
#include "prompts/create-token.hpp"
#include "prompts/enhance-prompt.hpp"
#include "redis.hpp"
#include "s3Client.hpp"
#include "types.hpp"
#include "uploader.hpp"
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Added for image decoding

// Enum for media types
enum MediaType {
}

// Configure rate limits per media type

// Token ownership requirements for generation

// Helper to check rate limits
  // Special handling for test environments
    // In test mode, we want to test different rate limit scenarios
    // Use the mint address to determine the rate limit behavior
      // Rate limit reached
      // Almost at rate limit
      // Default: plenty of generations left

  // Create a timeout for the database query

    // Count generations in the last 24 hours

    // Race the query against the timeout

    // If token ownership validation is enabled and user wallet is provided
      // Check if user owns enough tokens

    // Default to allowing the operation if rate limit check fails, but with 0 remaining
    // This prevents rate limit checks from blocking operations in case of DB issues

/**
 * Checks if a user owns the required minimum amount of tokens for generating content
 */
    // Special handling for test environments
      // Allow some test addresses to bypass the check

      // Test address to simulate not having enough tokens

      // Default to allowing in test mode

    // Check if the feature is enabled

    // Get minimum required token amount based on mode and media type

    // Access the database

      // First check if user is the token creator (creators always have access)

        // User is the token creator, allow generating

        // Fallback to blockchain check if Redis fails
      // ---> END CHANGE

      // If user is not in the token holders list (or Redis failed slightly earlier)
        // User is not a token holder according to cache, check the blockchain directly as fallback

      // User is in token holders list, check if they have enough tokens
      // const holder = holderQuery[0];
      // Convert minimum required to raw amount for comparison

        // Convert back to UI amount for the error message
      // Fall back to checking the blockchain directly if database check fails
    // Allow by default if there's an error in the function, but can be changed to false in production

/**
 * Fallback method to check token balance directly on the blockchain
 * Used when database lookup fails or when user is not in the token holders table
 */
    // Connect to Solana

    // Convert string addresses to PublicKey objects

    // Fetch token accounts with a simple RPC call

    // Calculate total token amount

    // Get token balances from all accounts

    // Sum up all token balances

    // Determine if user has enough tokens
    // Log the error but don't block operations due to a token check failure

    // Default to allowing if we can't check the balance
    // You may want to change this to false in production

// Function definition moved earlier
std::future<std::string> generateLyrics(std::optional<std::any> tokenMetadata, std::optional<std::string> stylePrompt);

std::future<std::string> generateStylePrompt(const std::string& userPrompt);

// Function definition moved earlier
std::string formatLyricsForDiffrhythm(const std::string& lyrics);

// Helper to generate media using fal.ai or Cloudflare Workers
std::future<void> generateMedia(std::optional<std::any> data);

// Media generation validation schema
  // Video specific options
  // Audio specific options
  // Common options
  // New options

// Token metadata generation validation schema

/**
 * Generate an image using Fal.ai API
 */
std::future<MediaGeneration> generateImage(const std::string& mint, const std::string& prompt, std::optional<std::string> negativePrompt, std::optional<std::string> creator);

/**
 * Generate a video using Fal.ai API
 */
std::future<MediaGeneration> generateVideo(const std::string& mint, const std::string& prompt, std::optional<std::string> negativePrompt, std::optional<std::string> creator);

/**
 * Get daily generation count and update if needed
 */
std::future<double> getDailyGenerationCount(const std::any& db, const std::string& mint, const std::string& creator);

// --- Define generateMetadata FIRST ---

// --- generateTokenOnDemand (updated) ---

    // Store in database

// Add the generatePreGeneratedTokens function
std::future<void> generatePreGeneratedTokens();

// Add the checkAndReplenishTokens function
std::future<void> checkAndReplenishTokens(std::optional<double> threshold);

// Helper function to generate an enhanced prompt using the token metadata
std::future<std::string> generateEnhancedPrompt(const std::string& userPrompt, std::optional<std::any> tokenMetadata, MediaType mediaType = MediaType.IMAGE);

// Function to generate additional images for a token
std::future<void> generateAdditionalTokenImages(const std::string& tokenMint, const std::string& description);
} // namespace elizaos
