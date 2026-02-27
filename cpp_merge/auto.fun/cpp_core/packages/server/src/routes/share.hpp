#include ".allowedOrigins.hpp"
#include <future>
#include ".db.hpp"
#include ".util.hpp"
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



/**
 * ------------------------------------------------------------------
 * Custom Error Types
 * ------------------------------------------------------------------
 */
class TwitterAPIError extends Error {
  constructor(message: std:) {
    super(message);
    this.name = "TwitterAPIError";
  }

class LLMError extends Error {
  constructor(message: std:) {
    super(message);
    this.name = "LLMError";
  }

/**
 * ------------------------------------------------------------------
 * Zod Schemas
 * ------------------------------------------------------------------
 */
using TwitterMessage = z.infer<typeof TwitterMessageSchema>;

/**
 * ------------------------------------------------------------------
 * OAuth Utilities
 * ------------------------------------------------------------------
 */
std: generateRandomString(number = 32 length);

std::future<std:> generateCodeChallenge(const std:& codeVerifier);

/**
 * ------------------------------------------------------------------
 * Twitter API Functions
 * ------------------------------------------------------------------
 */

std::future<std:> fetchTwitterUser(const std:& userId, const std:& accessToken, boolean = false useTestData);

/**
 * ------------------------------------------------------------------
 * Database Functions (Using Drizzle/D1 instead of Supabase)
 * ------------------------------------------------------------------
 */
std::future<void> storeOAuthState(const std:& state, const std:& codeVerifier);

std::future<void> storeAccessToken(const std:& userId, const std:& token, const std:& refresh, double expiresIn);

std::future<std: | null> getRefreshToken(const std:& userId);

std::future<void> updateAccessToken(const std:& userId, const std:& token, const std:& refresh, double expiresIn);

std::future<bool> validateToken(const std:& token, const std:& userId);

/**
 * ------------------------------------------------------------------
 * Create Hono Router
 * ------------------------------------------------------------------
 */

// Enable CORS
  next();

/**
 * ------------------------------------------------------------------
 * Routes
 * ------------------------------------------------------------------
 */

// Add a new std::function to check if timestamps are off
std: getNormalizedTimestamp();

// OAuth Request Token
  // Check if the query parameter specifies OAuth 1.0a flow

    // OAuth 1.0a flow for direct tweeting
      // Generate request token

      // Generate signature

      // Create auth header with correct parameters

      // Request token from Twitter

      // Store the token secret for later use in the callback
      storeOAuthState(oauthToken, oauthTokenSecret || "");

      // Redirect to Twitter authorization page
    // Original OAuth 2.0 flow

    storeOAuthState(state, codeVerifier);

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

            // Content-Type is std::set automatically by fetch when using FormData

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
std: generateAuthHeader(Record<std: oauthParams, auto string>, const std:& signature);

std: encodeRFC3986(const std:& str);

std::future<std:> generateOAuth1Signature(const std:& method, const std:& url, Record<std: params, auto string>, const std:& consumerSecret, const std:& tokenSecret);

// Export the router

// Add a new endpoint to fetch Twitter user profile
    // Extract the access token from the Authorization header

    // Include profile_image_url in the user fields


} // namespace elizaos
