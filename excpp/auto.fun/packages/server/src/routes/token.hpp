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
#include "externalToken.hpp"
#include "files.hpp"
#include "generation.hpp"
#include "mcap.hpp"
#include "ogImageGenerator.hpp"
#include "redis.hpp"
#include "s3Client.hpp"
#include "tokenSupplyHelpers.hpp"
#include "tools/normalizeParams.hpp"
#include "uploader.hpp"
#include "util.hpp"
#include "validators/global.hpp"
#include "validators/tokenHoldersQuery.hpp"
#include "validators/tokenQuery.hpp"
#include "validators/tokenSearchQuery.hpp"
#include "validators/tokenUpdateQuery.hpp"
#include "websocket-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

 // S3 Import

 // Buffer import

 // <<< Trying path relative to src

 // Import shared S3 client function

 // Import the S3 uploader

// --- Validation Function ---
std::future<void> validateQueryResults(std::optional<std::any> params, const std::optional<std::vector<Token>>& results, std::optional<{ mainQuerySql: string } // Optional parameter for SQL string> sqlStrings);

// --- Build Base Query (Filters) ---
// Adjust DB type if needed
PgSelect buildTokensBaseQuery(const std::any& db, std::optional<std::any> params);

// --- Build Count Query (Filters Only) ---
// Adjust DB type if needed
PgSelect buildTokensCountBaseQuery(const std::any& db, std::optional<std::any> params);

// Define the router (Env removed from Bindings)

// --- OG Image Endpoint ---
  // Match .png optionally
  // Get query params

    // 1. Check Redis for cached S3 URL (Skip if forceRefresh is true)
        // Use 302 Found for temporary redirect

    // 2. Cache Miss or Refresh Requested -> Generate Image
      // Redirect to a default OG image if generation fails
      // Consider hosting a static default image
      // You might want to return a 404 or 500 instead, depending on desired behavior
      // Alternative: return c.text('Failed to generate image', 500);

    // 3. Upload generated image to S3/R2
      // Use the updated uploadWithS3 function with cache control

      // Handle upload error (e.g., return 500 or default image)
      // Alternative: return c.text('Failed to store generated image', 500);

    // 4. Update Redis Cache with the *new* public URL

    // 5. Redirect to the newly uploaded image
    // General error fallback
    // Alternative: return c.text('Internal Server Error', 500);

// --- Endpoint to serve images from storage (S3 API) ---

    // Use the shared S3 client getter

    // Determine potential object key (might be generation or token image)

        // DEBUG: List files in the directory

// --- Endpoint to serve metadata JSON from storage (S3 API) ---

    // Use the shared S3 client getter

std::future<void> processSwapEvent(const std::any& swap, bool shouldEmitGlobal = true);

// Helper function to process token info after finding it on a network
std::future<void> processTokenInfo(PublicKey mintPublicKey, AccountInfo<Buffer> tokenInfo, Connection connection, const std::string& requestor);

// Helper to check token balance directly on blockchain
std::future<std::string> checkBlockchainTokenBalance(const std::any& c, const std::string& address, auto checkMultipleNetworks);

  // const limit = isSearching ? 5 : Math.min(requestedLimit, MAX_LIMIT);

    // @ts-ignore - Drizzle's execute() type might not be perfectly inferred

    // Define the two token addresses to prioritize

    /** Set cache for 20 seconds */

    // Get token data and potentially creator profile
          // Select all fields from tokens table
          // Select specific fields from users table, aliased

      // Don't cache 404s for the main token endpoint

    // Process the result - tokenResult[0] contains token fields and a creatorProfile object (which is null if no matching user was found)

    // Set default values for critical fields if they're missing

    // Make sure reserveAmount and reserveLamport have values

    // Update or set default values for missing fields

    // Calculate tokenPriceUSD in the same way as the old code
    // Ensure token.currentPrice is treated as SOL price per WHOLE token unit

    // Get virtualReserves and curveLimit from env or set defaults

    // Update virtualReserves and curveLimit

    // Calculate or update curveProgress using the original formula

    // Require authentication

    // Check if token already exists

      // Handle image upload if base64 data is provided

            // If no content type or not an image, skip processing

        // already data URI

      // Create token data with all required fields from the token schema

      // Convert imported and isToken2022 flags to numbers (0 or 1)

      // Insert with all required fields from the schema
          // Initialize other numeric fields explicitly to avoid DB defaults issues

      // For response, include just what we need

      // Trigger immediate updates for price and holders in the background
      // for both imported and newly created tokens

          // Fetch historical data in the background
          // Merge any immediately available market data
          // Continue even if webhook registration fails, especially locally

      // For non-imported tokens, generate additional images in the background
        // Use a simple async call if waitUntil is not available

    // Require authentication

      // holderCount, // Removed as update runs async

  // Permission check

  // Define allowed fields for update and prepare updateData
      // @ts-ignore

  // Update DB if needed

  // Update metadata in storage (S3 API) only if it's NOT an imported token
  // AND if there were actually relevant social fields updated (check updateData)
        // Get S3 details - client needed later, base URL for parsing

          // Check if URL starts with the expected public base URL (R2 or MinIO path style)
          // Fallback: try to infer based on common patterns if base URL doesn't match

        // Fetch existing metadata content

        // Only proceed with S3 update if fetch was successful
          // Update properties only if they were changed in updateData

  // Fetch updated token data to return

    // Emit WebSocket event

    // Get wallet address from query parameter

    // --- BEGIN REDIS CACHE CHECK (only if not forcing on-chain check) ---

          // Basic validation
    // --- END REDIS CACHE CHECK ---

    // Get token for decimals and creator information first

    // If token doesn't exist in our database, and we are forcing on-chain check
      // Pass c context to the helper function

    // If token doesn't exist in our database and not forcing on-chain
      // Don't cache 404s here

    // Check if user is the token creator

    // If forcing on-chain check, skip DB holder lookup
      // Pass c context to the helper function

        // Extract lastUpdated if available (assuming it's stored)
      // Continue, will likely result in 0 balance if not creator
    // ---> END CHANGE

    // if (holderQuery.length > 0) {
      // User is in the token holders list from Redis
      // const holder = holderQuery[0];

      // User is the creator but not in holders table, might have balance on-chain
      // Pass c context to the helper function
      // User is not in holders table and is not the creator

    // --- BEGIN REDIS CACHE SET (only if not forced on-chain) ---
        // Cache for a moderate duration (e.g., 60 seconds) as balances don't change instantly
    // --- END REDIS CACHE SET ---

  // Check if token is already imported

std::future<void> uploadImportImage(Context c);

// Add the upload-import-image route to the router

// Add this after other token routes

    // Get the form data

    // Validate file type

    // Check file size (10MB limit)

    // Read the file into a buffer

    // Determine file extension from content type

    // Create the object key

    // Upload to storage using existing function

/**
 * Fetches and prioritizes specific tokens at the beginning of the result set
 * @param db Database instance
 * @param serializableResults Current serialized token results
 * @param priorityTokenAddresses Array of token mint addresses to prioritize
 * @param limit Maximum number of tokens to return
 * @returns Modified token results with priority tokens at the beginning
 */


} // namespace elizaos
