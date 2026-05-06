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



class ScreenCaptureService {
public:
    ScreenCaptureService(VisionConfig config);
    Promise< getScreenInfo();
    std::future<ScreenCapture> captureScreen();
    std::future<void> captureScreenToFile(const std::string& outputPath);
    std::optional<ScreenTile> getActiveTile();
    std::vector<ScreenTile> getAllTiles();
    std::vector<ScreenTile> getProcessedTiles();

private:
    VisionConfig config_;
};


} // namespace elizaos
