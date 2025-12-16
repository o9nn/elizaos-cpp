#include "elizaos/core.hpp"
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
 * Represents media data containing a buffer of data and the media type.
 */
using MediaData = {

/**
 * Fetches media data from a list of attachments, supporting both HTTP URLs and local file paths.
 *
 * @param attachments - Array of Media objects containing URLs or file paths to fetch media from
 * @returns Promise that resolves with an array of MediaData objects containing the fetched media data and content type
 */

/**
 * Processes attachments by generating descriptions for supported media types.
 * Supports images and PDFs with automatic description generation using LLM.
 *
 * @param attachments - Array of attachments to process
 * @param runtime - Agent runtime for LLM access
 * @returns Returns a new array of processed attachments with added description, title, and text properties
 */


} // namespace elizaos
