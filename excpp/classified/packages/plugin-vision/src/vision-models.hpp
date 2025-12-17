#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "florence2-model.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Vision models for object detection and pose estimation

// Define types that are missing from types.ts
struct VisionModelConfig {
    std::optional<bool> enableObjectDetection;
    std::optional<bool> enablePoseDetection;
    std::string baseUrl;
    std::optional<std::string> apiKey;
    std::string model;
    double temperature;
    double maxTokens;
};

using Pose = std::variant<std::string, std::string, std::string, std::string, std::string>;

struct PoseLandmark {
    std::string name;
    double x;
    double y;
    double score;
};

class VisionModels {
public:
    VisionModels(IAgentRuntime runtime, std::optional<VisionModelConfig> config);
    std::future<void> initialize(VisionModelConfig config);
    bool hasObjectDetection();
    bool hasPoseDetection();
    std::future<std::vector<DetectedObject>> detectObjects(Buffer imageData, double width, double height, std::optional<std::string> description);
    std::vector<DetectedObject> enhancedObjectDetection(std::optional<std::string> description);
     generatePlausibleBoundingBox(const std::string& type, double index, double total);
    std::future<std::vector<PersonInfo>> detectPoses(Buffer imageData, double width, double height, std::optional<std::string> description);
    std::vector<PersonInfo> enhancedPoseDetection(std::optional<std::string> description);
    std::vector<PoseLandmark> generatePlausibleKeypoints(const std::any& boundingBox, Pose pose, const std::string& _facing);
    std::vector<PersonInfo> convertPosesToPersonInfo(const std::vector<poseDetection::Pose>& poses);
    Pose determinePoseFromKeypoints(const std::vector<poseDetection::Keypoint>& keypoints);
    std::string determineFacingDirection(const std::vector<poseDetection::Keypoint>& keypoints);
    std::future<void> dispose();

private:
    IAgentRuntime runtime_;
    VisionModelConfig config_;
};


} // namespace elizaos
