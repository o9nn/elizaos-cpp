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

declare module 'face-api.js' {
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
    std::vector<{ x: number; y: number }> positions;
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


  interface WithFaceDetection<_T> {
    detection: FaceDetection;
  }

  interface WithFaceLandmarks<_T> {
    landmarks: FaceLandmarks68;
  }

  interface WithFaceDescriptor<_T> {
    descriptor: Float32Array;
  }

  interface WithFaceExpressions<_T> {
    expressions: FaceExpressions;
  }

  interface WithAgeAndGender<_T> {
    age: number;
    gender: string;
    genderProbability: number;
  }

  using FullFaceDescription = WithFaceDetection<{}> &
    WithFaceLandmarks<{}> &
    WithFaceDescriptor<{}> &
    WithFaceExpressions<{}> &
    WithAgeAndGender<{}>;

  const env: {
    monkeyPatch: (config: any) => void;
  };

  const nets: {
    ssdMobilenetv1: {
      loadFromDisk: (path: string) => Promise<void>;
    };
    faceLandmark68Net: {
      loadFromDisk: (path: string) => Promise<void>;
    };
    faceRecognitionNet: {
      loadFromDisk: (path: string) => Promise<void>;
    };
    faceExpressionNet: {
      loadFromDisk: (path: string) => Promise<void>;
    };
    ageGenderNet: {
      loadFromDisk: (path: string) => Promise<void>;
    };
  };

  class SsdMobilenetv1Options {
    constructor(options: { minConfidence?: number; maxResults?: number });
  }

  ;
}

} // namespace elizaos
