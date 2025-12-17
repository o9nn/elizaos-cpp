#pragma once
#include <any>
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



struct WorkerStats {
    double fps;
    double frameCount;
    double lastUpdate;
};

class VisionWorkerManager {
public:
    VisionWorkerManager(VisionConfig config);
    std::future<void> initialize();
    std::future<void> startScreenCaptureWorker();
    std::future<void> startFlorence2Worker();
    std::future<void> startOCRWorker();
    void updateFlorence2Cache(const std::any& msg);
    void updateOCRCache(const std::any& _msg);
    std::optional<Florence2Result> readFlorence2Result(const std::string& tileId);
    std::optional<OCRResult> readOCRResult();
    std::optional<ScreenCapture> getLatestScreenCapture();
    EnhancedSceneDescription getLatestEnhancedScene();
    Array< generateTiles(double width, double height);
    std::unordered_map<std::string, WorkerStats> getWorkerStats();
    std::future<void> setDisplayIndex(double index);
    std::future<void> setTextRegions(const std::vector<std::any>& regions);
    std::future<void> stop();
    void handleWorkerLog(const std::string& workerName, const std::any& msg);
    std::future<void> restartScreenCaptureWorker();
    std::future<void> restartFlorence2Worker();
    std::future<void> restartOCRWorker();

private:
    VisionConfig config_;
    SharedArrayBuffer screenBuffer_;
    SharedArrayBuffer florence2ResultsBuffer_;
    SharedArrayBuffer ocrResultsBuffer_;
    Int32Array screenAtomicState_;
    DataView screenDataView_;
    DataView florence2ResultsView_;
    DataView ocrResultsView_;
};


} // namespace elizaos
