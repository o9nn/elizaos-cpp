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
#include "florence2-local.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class Florence2Model {
public:
    Florence2Model();
    std::future<void> initialize();
    std::future<Florence2Result> analyzeTile(ScreenTile tile);
    std::future<Florence2Result> analyzeImage(const std::vector<uint8_t>& imageBuffer);
    std::future<Florence2Result> mockAnalyze(ScreenTile tile);
    Array< detectUIElements(const std::vector<uint8_t>& imageBuffer);
    std::future<Florence2Result> mockAnalyzeBuffer(const std::vector<uint8_t>& _imageBuffer);
    std::string mapToUIElementType(const std::string& label);
    Promise< generateSceneGraph(const std::vector<ScreenTile>& tiles);
    std::string inferSpatialRelation(BoundingBox box1, BoundingBox box2);
    bool contains(BoundingBox box1, BoundingBox box2);
    bool overlaps(BoundingBox box1, BoundingBox box2);
    bool isInitialized();
    std::future<void> dispose();

private:
    Florence2Local localModel_;
};


} // namespace elizaos
