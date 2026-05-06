#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

  struct Box {
    double x;
    double y;
    double width;
    double height;
};

  struct FaceDetection {
    Box box;
    double score;
};

  struct FaceLandmarks68 {
};

  struct FaceExpressions {
    double neutral;
    double happy;
    double sad;
    double angry;
    double fearful;
    double disgusted;
    double surprised;
};

  struct AgeAndGenderPrediction {
    double age;
    std::string gender;
    double genderProbability;
};

  using FullFaceDescription = WithFaceDetection<{}> &

  class SsdMobilenetv1Options {
public:
};

  void detectAllFaces(const std::any& input, std::optional<std::any> options);;

} // namespace elizaos
