#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".allowedOrigins.hpp"
#include ".db.hpp"
#include ".util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * ------------------------------------------------------------------
 * Custom Error Types
 * ------------------------------------------------------------------
 */
class TwitterAPIError {
public:
    TwitterAPIError(const std::string& message);
};

class LLMError {
public:
    LLMError(const std::string& message);
};

/**
 * ------------------------------------------------------------------
 * Zod Schemas
 * ------------------------------------------------------------------
 */
using TwitterMessage = z::infer<typeof TwitterMessageSchema>;

/**
 * ------------------------------------------------------------------
 * OAuth Utilities
 * ------------------------------------------------------------------
 */
std::string generateRandomString(number = 32 length);

std::future<std::string> generateCodeChallenge(const std::string& codeVerifier);

/**
 * ------------------------------------------------------------------
 * Twitter API Functions
 * ------------------------------------------------------------------
 */

std::future<std::string> fetchTwitterUser(const std::string& userId, const std::string& accessToken, boolean = false useTestData);

/**
 * ------------------------------------------------------------------
 * Database Functions (Using Drizzle/D1 instead of Supabase)
 * ------------------------------------------------------------------
 */
std::future<void> storeOAuthState(const std::string& state, const std::string& codeVerifier);

std::future<void> storeAccessToken(const std::string& userId, const std::string& token, const std::string& refresh, double expiresIn);

std::future<void> updateAccessToken(const std::string& userId, const std::string& token, const std::string& refresh, double expiresIn);

std::future<bool> validateToken(const std::string& token, const std::string& userId);

/**
 * ------------------------------------------------------------------
 * Create Hono Router
 * ------------------------------------------------------------------
 */

// Enable CORS

/**
 * ------------------------------------------------------------------
 * Routes
 * ------------------------------------------------------------------
 */

// Add a new function to check if timestamps are off
std::string getNormalizedTimestamp();

// OAuth Request Token
  // Check if the query parameter specifies OAuth 1.0a flow

    // OAuth 1.0a flow for direct tweeting
      // Generate request token

      // Generate signature

      // Create auth header with correct parameters

      // Request token from Twitter

      // Store the token secret for later use in the callback

      // Redirect to Twitter authorization page
    // Original OAuth 2.0 flow

// OAuth 1.0a Access Token Callback

    // Get the stored token secret

    // Generate signature for access token request

    // Create auth header with correct parameters

    // Request access token

    // Return user-specific OAuth 1.0a credentials to the client

// OAuth Callback

    // Get OAuth 2.0 token

    // Get user info to determine Twitter user ID

    // We need to store OAuth 2.0 credentials but we'll return both OAuth 2.0 and user ID

    // Return both OAuth 2.0 token and user ID (for OAuth 1.0a generation on frontend)

// OAuth Refresh

// Process Handler

    // Extract the access token from the Authorization header

// New endpoint for OAuth 1.0a Media Upload
    // Get user's OAuth 1.0a credentials from headers

    // Get image data URL from the request body

    // Extract content type and base64 data

    // Convert base64 to ArrayBuffer, then Uint8Array

    // --- INIT ---
      // Optionally add media_category for video/gif: media_category: 'tweet_video' or 'tweet_gif'

    // --- APPEND ---

      // Regenerate timestamp and nonce for each APPEND request

      // Combine base OAuth params with current timestamp/nonce

      // Use current OAuth params for header generation as well

            // Content-Type is set automatically by fetch when using FormData

    // --- FINALIZE ---

      // Check for processing info in the response
          // Media might still be processing, return media_id but log warning

    // Even if processing, return the media_id for the frontend to use

// Tweet Handler (modified to remove conflicting FormData logic)
    // Get the user's OAuth 1.0a credentials from the request headers

    // Expect JSON body with text and media_ids

    // Generate OAuth 1.0a params for the tweet using user's credentials

    // Create tweet payload (using API v2 format)

    // Sign the request with user's credentials
    // Note: For API v2 POST requests with JSON body, only OAuth params are included in signature base string.

    // Handle error with detailed information
      // Try parsing error for more details
        /* Ignore if not JSON */

/**
 * ------------------------------------------------------------------
 * OAuth1 Signature Utilities
 * ------------------------------------------------------------------
 */
std::string generateAuthHeader(Record<string oauthParams, auto string>, const std::string& signature);

std::string encodeRFC3986(const std::string& str);

std::future<std::string> generateOAuth1Signature(const std::string& method, const std::string& url, Record<string params, auto string>, const std::string& consumerSecret, const std::string& tokenSecret);

// Export the router

// Add a new endpoint to fetch Twitter user profile
    // Extract the access token from the Authorization header

    // Include profile_image_url in the user fields


} // namespace elizaos
