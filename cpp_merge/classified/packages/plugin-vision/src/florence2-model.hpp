#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_FLORENCE2_MODEL_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_FLORENCE2_MODEL_H
#include "core.hpp"
// External dependency removed
#include "./types.h"
#include "./florence2-local.h"

class Florence2Model;

class Florence2Model : public object, public std::enable_shared_from_this<Florence2Model> {
public:
    using std::enable_shared_from_this<Florence2Model>::shared_from_this;
    boolean initialized = false;

    std::shared_ptr<Florence2Local> localModel;

    Florence2Model();
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> analyzeTile(std::shared_ptr<ScreenTile> tile);
    virtual std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> analyzeImage(std::shared_ptr<Buffer> imageBuffer);
    virtual std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> mockAnalyze(std::shared_ptr<ScreenTile> tile);
    virtual std::shared_ptr<Promise<array<object>>> detectUIElements(std::shared_ptr<Buffer> imageBuffer);
    virtual std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> mockAnalyzeBuffer(std::shared_ptr<Buffer> _imageBuffer);
    virtual string mapToUIElementType(string label);
    virtual std::shared_ptr<Promise<object>> generateSceneGraph(array<std::shared_ptr<ScreenTile>> tiles);
    virtual any inferSpatialRelation(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2);
    virtual boolean contains(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2);
    virtual boolean overlaps(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2);
    virtual boolean isInitialized();
    virtual std::shared_ptr<Promise<void>> dispose();
};

#endif
