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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define error interface for better type safety
struct RequestError {
    std::string message;
    std::optional<std::string> code;
    double status;
};

// Helper function to get content type from file extension
std::string getContentTypeFromExtension(const std::string& filename);

std::future<void> GET(NextRequest request, const std::any& context);

// Handle CORS preflight requests
std::future<void> OPTIONS();
} // namespace elizaos
