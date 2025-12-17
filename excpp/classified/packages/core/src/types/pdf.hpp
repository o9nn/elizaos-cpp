#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PdfExtractionResult {
    std::string text;
    double pageCount;
    std::optional<std::string> title;
    std::optional<std::string> author;
    std::optional<std::chrono::system_clock::time_point> createdAt;
    std::optional<std::chrono::system_clock::time_point> modifiedAt;
};

struct PdfGenerationOptions {
    std::optional<std::string> format;
    std::optional<std::string> orientation;
    std::optional<double> top;
    std::optional<double> bottom;
    std::optional<double> left;
    std::optional<double> right;
    std::optional<std::string> header;
    std::optional<std::string> footer;
};

struct PdfConversionOptions {
    std::optional<std::string> quality;
    std::optional<std::string> outputFormat;
    std::optional<bool> compression;
};

/**
 * Interface for PDF processing services
 */


} // namespace elizaos
