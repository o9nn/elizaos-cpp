#pragma once
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



class OCRService {
public:
    std::future<void> initialize();
    std::future<void> _initialize();
    std::future<OCRResult> extractText(Buffer imageBuffer);
    std::future<OCRResult> extractFromTile(ScreenTile tile);
    std::future<OCRResult> extractFromImage(Buffer imageBuffer);
    std::future<Buffer> preprocessImage(Buffer imageBuffer);
    OCRResult convertTesseractResult(Tesseract::RecognizeResult result);
    Promise< extractStructuredData(Buffer imageBuffer);
    Array< detectTables(OCRResult ocrResult);
    void for(auto const block of ocrResult.blocks);
    void if(auto sortedRows.length >= 2);
    Array< detectForms(OCRResult ocrResult);
    void for(auto let i = 0; i < ocrResult.blocks.length - 1; i++);
    Array< detectLists(OCRResult ocrResult);
    void for(auto const block of ocrResult.blocks);
    void if(auto listItems.length > 0);
    bool isInitialized();
    std::future<void> dispose();
};


} // namespace elizaos
