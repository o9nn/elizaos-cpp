#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_FLORENCE2-LOCAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_FLORENCE2-LOCAL_H
#include "core.h"
#include "@tensorflow/tfjs.h"
#include "@tensorflow/tfjs-backend-wasm.h"
#include "@elizaos/core.h"
#include "./types.h"
#include "sharp.h"
using sharp = _default;

class Florence2LocalConfig;
class Florence2Local;

class Florence2LocalConfig : public object, public std::enable_shared_from_this<Florence2LocalConfig> {
public:
    using std::enable_shared_from_this<Florence2LocalConfig>::shared_from_this;
    string modelPath;

    string modelUrl;

    string cacheDir;
};

class Florence2Local : public object, public std::enable_shared_from_this<Florence2Local> {
public:
    using std::enable_shared_from_this<Florence2Local>::shared_from_this;
    any model = nullptr;

    boolean initialized = false;

    std::shared_ptr<Florence2LocalConfig> config;

    double IMAGE_SIZE = 384;

    double VOCAB_SIZE = 51289;

    Florence2Local(std::shared_ptr<Florence2LocalConfig> config = undefined);
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> analyzeImage(std::shared_ptr<Buffer> imageBuffer);
    virtual std::shared_ptr<Promise<std::shared_ptr<tf::Tensor3D>>> preprocessImage(std::shared_ptr<Buffer> imageBuffer);
    virtual std::shared_ptr<Promise<std::shared_ptr<tf::Tensor>>> runInference(std::shared_ptr<tf::Tensor3D> input);
    virtual std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> parseModelOutput(std::shared_ptr<tf::Tensor> predictions);
    virtual string generateCaptionFromFeatures(any features);
    virtual array<string> extractTagsFromCaption(string caption);
    virtual std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> enhancedFallback(std::shared_ptr<Buffer> imageBuffer);
    virtual boolean isInitialized();
    virtual std::shared_ptr<Promise<void>> dispose();
};

#endif
