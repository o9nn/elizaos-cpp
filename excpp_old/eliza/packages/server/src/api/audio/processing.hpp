#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".shared/constants.js.hpp"
#include ".shared/file-utils.js.hpp"
#include ".shared/middleware.js.hpp"
#include ".shared/response-utils.js.hpp"
#include ".shared/uploads/index.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Securely validates a file path to prevent path traversal attacks
 */
std::string validateSecureFilePath(const std::string& filePath);

/**
 * Audio processing functionality - upload and transcription
 */

} // namespace elizaos
