#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    std::future<Florence2Result> analyzeImage(Buffer imageBuffer);
    std::future<Florence2Result> mockAnalyze(ScreenTile tile);
    Array< detectUIElements(Buffer imageBuffer);
    void if(auto !this.initialized);
    void catch(auto _modelError);
    void catch(auto error);
    std::future<Florence2Result> mockAnalyzeBuffer(Buffer _imageBuffer);
    std::string mapToUIElementType(const std::string& label);
    Promise< generateSceneGraph(const std::vector<ScreenTile>& tiles);
    void for(auto const tile of tiles);
    void for(auto let i = 0; i < nodes.length; i++);
    std::optional<std::string> inferSpatialRelation(BoundingBox box1, BoundingBox box2);
    bool contains(BoundingBox box1, BoundingBox box2);
    bool overlaps(BoundingBox box1, BoundingBox box2);
    bool isInitialized();
    std::future<void> dispose();

private:
    Florence2Local localModel_;
};


} // namespace elizaos
