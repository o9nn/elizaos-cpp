#include "service.hpp"
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



struct PdfExtractionResult {
    std::string text;
    double pageCount;
    std::optional<{> metadata;
    std::optional<std::string> title;
    std::optional<std::string> author;
    std::optional<Date> createdAt;
    std::optional<Date> modifiedAt;
};

struct PdfGenerationOptions {
    std::optional<'A4' | 'A3' | 'Letter'> format;
    std::optional<'portrait' | 'landscape'> orientation;
    std::optional<{> margins;
    std::optional<double> top;
    std::optional<double> bottom;
    std::optional<double> left;
    std::optional<double> right;
    std::optional<std::string> header;
    std::optional<std::string> footer;
};

struct PdfConversionOptions {
    std::optional<'high' | 'medium' | 'low'> quality;
    std::optional<'pdf' | 'pdf/a'> outputFormat;
    std::optional<bool> compression;
};

/**
 * Interface for PDF processing services
 */

  /**
   * Extract text and metadata from a PDF file
   * @param pdfPath - Path to the PDF file or buffer
   * @returns Promise resolving to extracted text and metadata
   */

  /**
   * Generate a PDF from HTML content
   * @param htmlContent - HTML content to convert to PDF
   * @param options - PDF generation options
   * @returns Promise resolving to PDF buffer
   */

  /**
   * Convert a document to PDF format
   * @param filePath - Path to the document file
   * @param options - Conversion options
   * @returns Promise resolving to PDF buffer
   */

  /**
   * Merge multiple PDF files into one
   * @param pdfPaths - Array of PDF file paths or buffers
   * @returns Promise resolving to merged PDF buffer
   */

  /**
   * Split a PDF into individual pages
   * @param pdfPath - Path to the PDF file or buffer
   * @returns Promise resolving to array of page buffers
   */

} // namespace elizaos
