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
#include "worker-logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WorkerConfig {
    std::optional<double> displayIndex;
    std::optional<bool> captureAllDisplays;
    std::optional<double> targetFPS;
    double sharedBufferSize;
};

struct DisplayInfo {
    std::string id;
    std::string name;
    double width;
    double height;
    double x;
    double y;
    std::optional<bool> isPrimary;
};

class ScreenCaptureWorker {
public:
    ScreenCaptureWorker(WorkerConfig config, SharedArrayBuffer sharedBuffer);
    std::future<void> initialize();
    std::future<std::vector<DisplayInfo>> getDisplays();
    std::future<void> run();
    std::future<void> captureFrame();
    std::future<void> captureScreenToFile(const std::string& outputPath, DisplayInfo display);
    void stop();

private:
    WorkerConfig config_;
    SharedArrayBuffer sharedBuffer_;
    DataView dataView_;
    std::vector<int32_t> atomicState_;
};

// Worker entry point

  // Handle messages from main thread

  // Run the worker

} // namespace elizaos
