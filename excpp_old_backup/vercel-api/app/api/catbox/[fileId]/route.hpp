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



// Define error interface for better type safety
struct RequestError {
    std::string message;
    std::optional<std::string> code;
    std::optional<{> response;
    double status;
};

// Helper function to get content type from file extension

std::future<void> GET(NextRequest request, { params: Promise<{ fileId: string }> } context);

// Handle CORS preflight requests
std::future<void> OPTIONS();
} // namespace elizaos
