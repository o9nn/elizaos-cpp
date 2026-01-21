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
#include "env.hpp"
#include "s3Client.hpp"
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// 

 // Import crypto if not already available globally in the environment
 // Ensure Buffer is available
 // Import the shared S3 client function

// Store file mapping in a local cache for development

// Log uploaded files to an in-memory cache only
void logUploadedFile(const std::string& objectKey, const std::string& publicUrl);

// Get all logged files

std::future<void> uploadWithS3(std::any options = {});

// Function to upload a generated image to a predictable path for a token
std::future<void> uploadGeneratedImage(double generationNumber, std::any options = {});

} // namespace elizaos
