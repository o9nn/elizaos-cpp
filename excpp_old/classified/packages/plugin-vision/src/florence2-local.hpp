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



struct Florence2LocalConfig {
    std::optional<std::string> modelPath;
    std::optional<std::string> modelUrl;
    std::optional<std::string> cacheDir;
};

class Florence2Local {
public:
    Florence2Local(std::optional<Florence2LocalConfig> config);
    std::future<void> initialize();
    std::future<Florence2Result> analyzeImage(Buffer imageBuffer);
    std::future<tf::Tensor3D> preprocessImage(Buffer imageBuffer);
    std::future<tf::Tensor> runInference(tf::Tensor3D input);
    std::future<Florence2Result> parseModelOutput(tf::Tensor predictions);
    std::string generateCaptionFromFeatures(const std::any& features);
    std::vector<std::string> extractTagsFromCaption(const std::string& caption);
    std::future<Florence2Result> enhancedFallback(Buffer imageBuffer);
    bool isInitialized();
    std::future<void> dispose();

private:
    Florence2LocalConfig config_;
};


} // namespace elizaos
