#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Type definition for a code block placeholder
 */
struct CodeBlockPlaceholder {
    std::string placeholder;
    std::string content;
};

/**
 * Type for reconstructed response
 */
struct ReconstructedResponse {
    std::string type;
    std::optional<std::string> thought;
    std::optional<std::string> message;
    std::string language;
    std::string code;
};

/**
 * Type for reflection schema response
 */
struct ReflectionResponse {
    std::string thought;
    std::vector<unknown> facts;
    std::vector<unknown> relationships;
    std::string rawContent;
};

/**
 * Type for unstructured response
 */
struct UnstructuredResponse {
    std::string type;
    std::string content;
};

/**
 * Type for JSON extraction result
 */
using ExtractedJSON = std::variant<, std::unordered_map<std::string, unknown>, ReconstructedResponse, ReflectionResponse, UnstructuredResponse>;

/**
 * Helper function to ensure reflection response has all required properties
 */
  // Only process if it's a reflection schema request

  // Check if it's an object with potentially missing reflection properties
    // Create a new object with required properties

/**
 * Enhanced function to extract and parse JSON from LLM responses
 * Handles various response formats including mixed markdown and JSON with code blocks
 */
    // First attempt: Try direct JSON parsing

    // Try JSONRepair first

    // Check if we have a valid JSON structure with embedded code blocks
    // This specifically addresses the case where Anthropic returns a valid JSON object
    // that contains markdown code blocks inside string values

      // Replace code blocks with escaped versions to preserve them in the JSON
        // First, try to preserve the code blocks by temporarily replacing them

        // Try parsing with placeholders
          // Try JSONRepair first
          // If JSONRepair fails, try direct parsing

        // Restore code blocks in the parsed object

    // Try to extract JSON from code blocks
      // First priority: explicit JSON code blocks

      // Second priority: any code block that contains JSON-like content

        // Try parsing the extracted content
          // Try with JSONRepair

    // Look for JSON structure outside of code blocks
      // Try to find JSON-like content in the text
      // This regex looks for content that starts with { and ends with }

      // If no direct match, try to find the largest JSON-like structure

        // Sort matches by length (descending) to try the largest JSON-like structure first

        // Try parsing the extracted JSON
          // Try with JSONRepair

    // Try to manually extract a "thought"/"message" structure which is common
      // Extract thought/message pattern if present

          // If no message was found but we have a thought, try to use the rest of the content as message
            // Remove the thought part from the content

          // Look for code blocks in the remaining content

            // Remove code blocks from the remaining content

          // Use the cleaned remaining content as message

      // For reflection schema-like structure

        // Try to extract thought

        // Attempt to extract facts and relationships would go here
        // This would require more complex parsing logic for arrays

    // Last resort: Return a structured object with the raw text

// Future config schema for generateObject
/**
 * Configuration schema for Anthropic plugin.
 * This will be used when we switch to generateObject instead of generateText.
 */
// Define a configuration schema for the Anthropics plugin.
// const configSchema = z.object({
// 	ANTHROPIC_API_KEY: z.string().min(1, "Anthropic API key is required"),
// 	ANTHROPIC_SMALL_MODEL: z.string().optional(),
// 	ANTHROPIC_LARGE_MODEL: z.string().optional(),
// });

} // namespace elizaos
