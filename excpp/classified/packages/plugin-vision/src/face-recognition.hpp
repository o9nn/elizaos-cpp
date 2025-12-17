#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    Array< detectFaces(Buffer imageData, double width, double height);
    Promise< recognizeFace(Float32Array descriptor);
    std::future<std::string> addOrUpdateFace(Float32Array descriptor, std::optional<std::optional<FaceProfile>> attributes);
    FaceProfile getFaceProfile(const std::string& profileId);
    std::vector<FaceProfile> getAllProfiles();
    double euclideanDistance(const std::variant<Float32Array, std::vector<double>>& a, const std::vector<double>& b);
    std::future<void> saveFaceLibrary(const std::string& path);
    std::future<void> loadFaceLibrary(const std::string& path);
};


} // namespace elizaos
