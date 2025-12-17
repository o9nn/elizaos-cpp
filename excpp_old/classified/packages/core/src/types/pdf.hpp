#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
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
    std::optional<Date> createdAt;
    std::optional<Date> modifiedAt;
};

struct PdfGenerationOptions {
    std::optional<std::variant<'A4', 'A3', 'Letter'>> format;
    std::optional<std::variant<'portrait', 'landscape'>> orientation;
    std::optional<double> top;
    std::optional<double> bottom;
    std::optional<double> left;
    std::optional<double> right;
    std::optional<std::string> header;
    std::optional<std::string> footer;
};

struct PdfConversionOptions {
    std::optional<std::variant<'high', 'medium', 'low'>> quality;
    std::optional<std::variant<'pdf', 'pdf/a'>> outputFormat;
    std::optional<bool> compression;
};

/**
 * Interface for PDF processing services
 */


} // namespace elizaos
