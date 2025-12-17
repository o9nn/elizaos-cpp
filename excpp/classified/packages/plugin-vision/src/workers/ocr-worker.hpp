#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".ocr-service.hpp"
#include "worker-logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WorkerConfig {
    bool processFullScreen;
    double tileSize;
    double y;
    double width;
};

struct SharedMetadata {
    double frameId;
    double width;
    double height;
    double displayIndex;
    double timestamp;
};

class OCRWorker {
public:
    OCRWorker(WorkerConfig config, SharedArrayBuffer sharedBuffer, SharedArrayBuffer resultsBuffer);
    std::future<void> initialize();
    std::future<void> run();
    std::future<void> processFrame();
    std::future<Buffer> extractFullScreenBuffer(SharedMetadata metadata);
    std::future<Buffer> extractRegionBuffer(const std::any& region, SharedMetadata metadata);
    std::future<void> writeResultsToBuffer(const std::vector<OCRResult>& results, double frameId);
    void stop();
    std::future<void> dispose();
    void updateTextRegions(const std::vector<std::any>& regions);

private:
    WorkerConfig config_;
    SharedArrayBuffer sharedBuffer_;
    DataView dataView_;
    Int32Array atomicState_;
    SharedArrayBuffer resultsBuffer_;
    DataView resultsView_;
    OCRService ocrService_;
};

// Worker entry point

  // Handle messages from main thread

  // Run the worker

} // namespace elizaos
