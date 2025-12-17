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
#include "florence2-model.hpp"
#include "worker-logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WorkerConfig {
    double tileSize;
    std::optional<std::vector<double>> priorityTiles;
};

struct SharedMetadata {
    double frameId;
    double width;
    double height;
    double displayIndex;
    double timestamp;
};

class Florence2Worker {
public:
    Florence2Worker(WorkerConfig config, SharedArrayBuffer sharedBuffer, SharedArrayBuffer resultsBuffer);
    std::future<void> initialize();
    std::future<void> run();
    std::future<void> processFrame();
    std::vector<ScreenTile> calculateTiles(double width, double height);
    std::future<std::vector<uint8_t>> extractTileFromSharedBuffer(ScreenTile tile, SharedMetadata metadata);
    std::future<void> writeResultToBuffer(const std::string& tileId, Florence2Result result, double frameId);
    void stop();
    std::future<void> dispose();

private:
    WorkerConfig config_;
    SharedArrayBuffer sharedBuffer_;
    DataView dataView_;
    std::vector<int32_t> atomicState_;
    SharedArrayBuffer resultsBuffer_;
    DataView resultsView_;
    Florence2Model florence2_;
};

// Worker entry point

  // Handle messages from main thread

  // Run the worker

} // namespace elizaos
