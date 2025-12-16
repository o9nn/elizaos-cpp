#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

declare module '@tensorflow-models/pose-detection' {
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
    PoseNet = 'PoseNet',
    MoveNet = 'MoveNet',
    BlazePose = 'BlazePose',
  }

  struct PosenetModelConfig {
    std::optional<'MobileNetV1' | 'ResNet50'> architecture;
    std::optional<double> outputStride;
    std::optional<{ width: number; height: number }> inputResolution;
    std::optional<double> multiplier;
};


  function createDetector(
    model: SupportedModels,
    config?: PosenetModelConfig | any
  ): Promise<PoseDetector>;
}

} // namespace elizaos
