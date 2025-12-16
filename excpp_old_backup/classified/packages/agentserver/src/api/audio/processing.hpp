#include ".shared/constants.hpp"
#include ".shared/file-utils.hpp"
#include ".shared/middleware.hpp"
#include ".shared/response-utils.hpp"
#include ".shared/uploads/index.hpp"
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
 * Securely validates a file path to prevent path traversal attacks
 */
std::string _validateSecureFilePath(const std::string& filePath);

/**
 * Audio processing functionality - upload and transcription
 */

} // namespace elizaos
