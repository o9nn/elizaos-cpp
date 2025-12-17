#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class OCRService {
public:
    std::future<void> initialize();
    std::future<void> _initialize();
    std::future<OCRResult> extractText(const std::vector<uint8_t>& imageBuffer);
    std::future<OCRResult> extractFromTile(ScreenTile tile);
    std::future<OCRResult> extractFromImage(const std::vector<uint8_t>& imageBuffer);
    std::future<std::vector<uint8_t>> preprocessImage(const std::vector<uint8_t>& imageBuffer);
    OCRResult convertTesseractResult(Tesseract::RecognizeResult result);
    Promise< extractStructuredData(const std::vector<uint8_t>& imageBuffer);
    Array< detectTables(OCRResult ocrResult);
    Array< detectForms(OCRResult ocrResult);
    Array< detectLists(OCRResult ocrResult);
    bool isInitialized();
    std::future<void> dispose();
};


} // namespace elizaos
