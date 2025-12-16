#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Extracts text content from a file buffer based on its content type.
 * Supports DOCX, plain text, and provides a fallback for unknown types.
 * PDF should be handled by `convertPdfToTextFromBuffer`.
 */
std::future<std::string> extractTextFromFileBuffer(Buffer fileBuffer, const std::string& contentType, string // For logging and context originalFilename);

/**
 * Converts a PDF file buffer to text content.
 * Requires pdfjs-dist to be properly configured, especially its worker.
 */
/**
 * Converts a PDF Buffer to text with enhanced formatting preservation.
 *
 * @param {Buffer} pdfBuffer - The PDF Buffer to convert to text
 * @param {string} [filename] - Optional filename for logging purposes
 * @returns {Promise<string>} Text content of the PDF
 */
std::future<std::string> convertPdfToTextFromBuffer(Buffer pdfBuffer, std::optional<std::string> filename);

/**
 * Determines if a file should be treated as binary based on its content type and filename
 * @param contentType MIME type of the file
 * @param filename Original filename
 * @returns True if the file should be treated as binary (base64 encoded)
 */
bool isBinaryContentType(const std::string& contentType, const std::string& filename);

/**
 * Check if the input is a TextItem.
 *
 * @param item - The input item to check.
 * @returns A boolean indicating if the input is a TextItem.
 */

/**
 * Normalizes an S3 URL by removing query parameters (signature, etc.)
 * This allows for consistent URL comparison regardless of presigned URL parameters
 * @param url The S3 URL to normalize
 * @returns The normalized URL containing only the origin and pathname
 */
std::string normalizeS3Url(const std::string& url);

/**
 * Fetches content from a URL and converts it to base64 format
 * @param url The URL to fetch content from
 * @returns An object containing the base64 content and content type
 */

    // Fetch the URL with timeout

    // Get content type from response headers

    // Get content as ArrayBuffer

    // Convert to base64

bool looksLikeBase64(std::optional<std::optional<std::string>> content);

/**
 * Generates a consistent UUID for a document based on its content.
 * Takes the first N characters/lines of the document and creates a hash-based UUID.
 * This ensures the same document always gets the same ID, preventing duplicates.
 *
 * @param content The document content (text or base64)
 * @param agentId The agent ID to namespace the document
 * @param options Optional configuration for ID generation
 * @returns A deterministic UUID based on the content
 */
std::string generateContentBasedId(const std::string& content, const std::string& agentId, std::optional<std::any> options);

/**
 * Extracts the first N lines from text content for ID generation
 * @param content The full text content
 * @param maxLines Maximum number of lines to extract
 * @returns The extracted lines as a single string
 */
std::string extractFirstLines(const std::string& content, number = 10 maxLines);

} // namespace elizaos
