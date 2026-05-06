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
#include "adminAddresses.hpp"
#include "db.hpp"
#include "files.hpp"
#include "logger.hpp"
#include "s3Client.hpp"
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Import shared S3 client

// Define the router with environment typing

// Middleware to check if user has admin privileges

// New middleware that checks for admin OR moderator status

    // Set a context variable to indicate this is a full admin

  // Check if user is a moderator

  // Mark this user as a moderator but not a full admin

// Route to update a token's social links

    // Check if token exists

    // Update token with the new social links

    // Get the updated token data

// Route to set featured flag on tokens

      // Check if token exists

      // Get the updated token data

// Route to set verified flag on tokens

      // Check if token exists

      // Get the updated token data

// Route to set hidden flag on tokens

    // Check if token exists

    // Update the hidden field directly

    // Get the updated token data

// Route to set a user to suspended

      // Check if user exists

      // Update the suspended field directly

      // Get the updated user data

      // For backward compatibility, also check if the name has the [SUSPENDED] prefix
      // and include a suspended property in the response

// Route to get a single user by address

    // Get user from database

    // Get suspended status from the suspended field

    // For backward compatibility, also check if the name has the [SUSPENDED] prefix

    // Use the suspended field if it's set, otherwise fall back to the name check

    // Add empty arrays for tokensCreated, tokensHeld, and transactions if they don't exist
    // This prevents "Cannot read properties of undefined (reading 'length')" errors

// Route to get admin statistics

    // Get total user count

    // Get total token count

    // Calculate 24h volume by summing the volume24h field from all tokens
    // In a real app, this would likely come from a transactions table with proper date filtering

// Route to retrieve users in a paginated way

    // Get search params for filtering

    // Use a shorter timeout for test environments

    // Create a timeout promise to prevent hanging

    // Prepare a basic query
        // Get all columns from the users table programmatically

        // Start with a basic query

        // Apply filters for suspended users using the suspended field
        // For backward compatibility, also check the name prefix

          // This is a simplified implementation - in production you'd use a proper search mechanism

        // Apply sorting - map frontend sort values to actual DB columns

        // Use the mapped column or default to createdAt

        // Apply pagination

        // Execute the query

      return Number(totalCountResult[0]?.count || 0);

    // Try to execute the query with a timeout

    // Add empty arrays for tokensCreated, tokensHeld, and transactions for each user

    // Return empty results rather than error

  // Fetch token data to check ownership using Drizzle syntax

// Create owner router for token owner specific endpoints

// Route to update a token's social links (owner version)

    // Update social links

    // Get the updated token data

// --- Build Base Query for Admin Tokens (Omits hidden filter) ---
PgSelect buildAdminTokensBaseQuery(const std::any& db, std::optional<std::any> params);

// --- Build Count Query for Admin Tokens (Omits hidden filter) ---
PgSelect buildAdminTokensCountBaseQuery(const std::any& db, std::optional<std::any> params);

// --- NEW: Route to retrieve ALL tokens (including hidden) for Admin Panel ---
  // --- Parameter Reading (similar to /api/tokens) ---

  // Adjust sortBy logic for admin if needed (e.g., different default)

  // Handle frontend-specific sort keys if passed directly

  // No caching for the admin endpoint to ensure freshness

  // --- Get max values for featured sort (if used) ---

  // --- Build Base Queries using ADMIN builders ---

  // --- Apply Sorting to Main Query ---

  // --- Apply Pagination ---

  // --- Execute Queries ---
    // Cast to any[] as a workaround for persistent Drizzle/TS type inference issues

  // --- Process and Return ---

  // Map results to plain objects with necessary conversions

      // Convert BigInts to strings
        // Add other conversions like Dates if needed

      // Ensure flags are booleans

// --- END NEW ADMIN GET TOKENS ROUTE ---

// --- Route to update core token details (name, ticker, image, url, description) ---

    // Add description to destructuring

    // Basic validation (can be more sophisticated)
    // Allow empty string for image/url/description to clear the field

    // Check if token exists

    // Prepare update data - only include fields that were provided in the request

    // Only update if there are changes other than lastUpdated
      // Return current token data even if no changes were made?

    // Update token with the new details

    // Get the updated token data to return

// --- END PUT ROUTE ---

// --- NEW: Route to update metadata JSON ---

      // Get updated metadata content from request body

      // Get token data, including URL and imported status

      // --- Check if token is imported ---

      // --- Check if original metadata URL exists ---

      // --- Extract S3 Object Key from URL ---
      // Get public base URL from shared client to help parse

      // Define potential prefixes

        // Check primary case: starts with current base URL
        // Check legacy/hardcoded R2 prefix
        // Check if it looks like a MinIO path URL
        // Fallback for local API path or other unknowns
            // Attempt to construct a likely S3 key (assuming token-metadata prefix)

      // --- Upload updated metadata to S3 ---
      // Use shared client

      // --- Add Logging Here ---
      // --- End Logging ---

      // Optionally, update the token's lastUpdated timestamp in the DB

// --- END METADATA UPDATE ROUTE ---

// --- NEW: Route to upload a new image for a token ---

    // Basic regex to extract content type and base64 data

    // Check if token exists

    // Generate filename (e.g., using mint and timestamp for uniqueness)

    // --- Upload Image using shared function ---
    // --- End Image Upload ---

    // --- Update Token in Database ---
    // --- End DB Update ---

    // Get the fully updated token data to return

// --- END IMAGE UPLOAD ROUTE ---

// --- NEW: Route to DELETE a token ---

    // Check if token exists before attempting delete (optional but good practice)

      // Return success even if not found, as the desired state (deleted) is achieved
      // Alternatively, return 404: return c.json({ error: "Token not found" }, 404);

    // Delete the token

      // Should not happen if the select check passed, but handle just in case

    // Optionally: Add logic here to delete associated data (e.g., holders, S3 assets) if needed

// --- END DELETE TOKEN ROUTE ---

// Routes for managing moderators (only accessible by full admins)
// Get list of current moderators

    // Get all moderator users

// Add a new moderator

    // Check if user exists

    // If user doesn't exist, create them

      // Get the newly created user
      // Update existing user to be a moderator

      // Get updated user data

// Remove a moderator

    // Don't allow removing admins from moderator status

    // Check if user exists and is a moderator

    // Update user to remove moderator status


} // namespace elizaos
