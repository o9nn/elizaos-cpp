#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".data/tags.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Preprocesses file paths for optimal matching
 * - Normalizes path separators
 * - Trims whitespace
 *
 * @param content The file path content string
 * @returns The preprocessed content string
 */
std::string preprocessFilePathContent(const std::string& content);

/**
 * Preprocesses text content (commit messages, PR titles, etc.)
 * - Trims whitespace
 *
 * @param content The text content string
 * @returns The preprocessed content string
 */
std::string preprocessTextContent(const std::string& content);

/**
 * Preprocesses code content
 * - Trims whitespace
 * - Normalizes line endings
 *
 * @param content The code content string
 * @returns The preprocessed content string
 */
std::string preprocessCodeContent(const std::string& content);

/**
 * Preprocesses label content
 * - Trims whitespace
 * - Converts to lowercase
 *
 * @param content The label content string
 * @returns The preprocessed content string
 */
std::string preprocessLabelContent(const std::string& content);

/**
 * Preprocesses content based on its type.
 * Returns a new MatchContent object with the processed content string.
 *
 * @param content The content object to preprocess
 * @returns The preprocessed content object
 */
MatchContent preprocessContent(MatchContent content);

} // namespace elizaos
