#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".components/icons.hpp"
#include ".types/form.type.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import the worker using Vite's ?worker syntax
 // Added import

// Use the token supply and virtual reserves from environment or fallback to defaults

// Tab types
enum FormTab {
}

// LocalStorage key for tab state

struct UploadResponse {
    bool success;
    std::string imageUrl;
    std::string metadataUrl;
};

struct GenerateImageResponse {
    bool success;
    std::string mediaUrl;
    double remainingGenerations;
    std::string resetTime;
};

struct PreGeneratedTokenResponse {
    bool success;
    std::string id;
    std::string name;
    std::string ticker;
    std::string description;
    std::string prompt;
    std::optional<std::string> image;
    std::string createdAt;
    double used;
};

struct GenerateMetadataResponse {
    bool success;
    std::string name;
    std::string symbol;
    std::string description;
    std::string prompt;
};

struct UploadImportImageResponse {
    bool success;
    std::string imageUrl;
};

// Define tokenData interface
struct TokenSearchData {
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<std::string> description;
    std::optional<std::string> creator;
    std::optional<std::vector<std::string>> creators;
    std::optional<std::string> image;
    std::string mint;
    std::optional<std::string> twitter;
    std::optional<std::string> telegram;
    std::optional<std::string> website;
    std::optional<std::string> discord;
    std::optional<std::string> metadataUri;
    std::optional<bool> isCreator;
    std::optional<std::string> updateAuthority;
};

// Vanity Generator Types (Copied from testing.tsx)
using VanityResult = {
using WorkerMessage = std::variant<, {

// Base58 characters

// Form Components

            // Call the original onFocus if it exists
            // Call the original onBlur if it exists

  // Expose the setPreview function to the parent component

  // Update preview from imageUrl prop if provided

  // Debounced prompt change handler

  // Update lastGeneratedImage only when preview changes

  // Pass prompt functions to parent only once on mount

  // Handle file selection

        // Check if file is an image

        // Check file size (limit to 5MB)

        // Create a preview URL

        // Pass the file to parent

  // Handle drag & drop

        // Check if file is an image

        // Check file size (limit to 5MB)

        // Create a preview URL

        // Pass the file to parent

  // Trigger file input click

  // Remove image
    // Only allow removing images in Manual mode

  // Cleanup timeout on unmount

  // Don't render anything for IMPORT tab

              // Manual mode - File upload UI
              // Auto mode - Prompt text area

// Image upload function
  // Determine a safe filename based on token metadata

  // Get the image type from the data URL

  // Determine file extension from content type

  // Get auth token from localStorage with quote handling

  // Prepare headers

    // Specifically handle authentication errors

  // Verify metadata URL exists, if not create a fallback

    // Generate a fallback URL using the mint address or a UUID

// Main Form Component

} // namespace elizaos
