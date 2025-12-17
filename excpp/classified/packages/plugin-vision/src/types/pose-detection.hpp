#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

  struct Keypoint {
    double x;
    double y;
    std::optional<double> z;
    std::optional<double> score;
    std::optional<std::string> name;
};

  struct Pose {
    std::vector<Keypoint> keypoints;
    std::optional<double> score;
};

  struct PoseDetector {
};

  enum SupportedModels {
  }

  struct PosenetModelConfig {
    std::optional<std::string> architecture;
    std::optional<double> outputStride;
    std::optional<double> multiplier;
};


} // namespace elizaos
