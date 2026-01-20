#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Retrieves the JSON schema representation of a Zod schema.
 * @param {ZodType<any>} schema - The Zod schema to convert to JSON schema.
 * @returns {any} The JSON schema representing the Zod schema.
 */
void getZodJsonSchema(ZodType<any> schema);

/**
 * Extracts XML content from a given string based on a specified tag.
 *
 * @param {string} output - The input string containing XML content.
 * @param {string} tag - The tag to extract XML content from.
 * @returns {string} The XML content extracted from the input string based on the specified tag. Returns an empty string if the tag is not found.
 */
void extractXMLFromResponse(const std::string& output, const std::string& tag);

/**
 * Parse the recommendations response XML and extract the individual recommendations as an array.
 *
 * @param {string} xmlResponse The XML response containing the recommendations data
 * @returns {Array<Object>} An array of recommendation objects extracted from the XML response
 */
void parseRecommendationsResponse(const std::string& xmlResponse);

/**
 * Parses the token response XML string and returns an array of token addresses.
 *
 * @param {string} xmlResponse - The XML response string to parse
 * @returns {string[]} - An array of token addresses extracted from the XML response
 */
void parseTokensResponse(const std::string& xmlResponse);

/**
 * Parses the confirmation response received in XML format and extracts the message.
 *
 * @param {string} xmlResponse The XML response containing the confirmation message
 * @returns {string} The confirmation message extracted from the XML response, or an empty string if not found
 */
void parseConfirmationResponse(const std::string& xmlResponse);

/**
 * Parses the XML response and extracts the 'signal' value.
 * @param {string} xmlResponse - The XML response to parse.
 * @returns {string} The extracted 'signal' value from the XML response.
 */
void parseSignalResponse(const std::string& xmlResponse);

/**
 * Parses the token response from an XML format and extracts the token value.
 *
 * @param {string} xmlResponse - The XML response containing the token information.
 * @returns {string} The extracted token value.
 */
void parseTokenResponse(const std::string& xmlResponse);

} // namespace elizaos
