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
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Dynamic imports for optional dependencies

std::future<void> initializeCanvas();

class FaceRecognition {
public:
    std::future<void> initialize();
    Array< detectFaces(const std::vector<uint8_t>& imageData, double width, double height);
    Promise< recognizeFace(const std::vector<float>& descriptor);
    std::future<std::string> addOrUpdateFace(const std::vector<float>& descriptor, std::optional<std::optional<FaceProfile>> attributes);
    FaceProfile getFaceProfile(const std::string& profileId);
    std::vector<FaceProfile> getAllProfiles();
    double euclideanDistance(const std::variant<std::vector<float>, std::vector<double>>& a, const std::vector<double>& b);
    std::future<void> saveFaceLibrary(const std::string& path);
    std::future<void> loadFaceLibrary(const std::string& path);
};


} // namespace elizaos
