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
#include "audio-capture-stream.hpp"
#include "audio-capture.hpp"
#include "elizaos/core.hpp"
#include "entity-tracker.hpp"
#include "face-recognition.hpp"
#include "florence2-model.hpp"
#include "ocr-service.hpp"
#include "screen-capture.hpp"
#include "types.hpp"
#include "vision-models.hpp"
#include "vision-worker-manager.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Vision service for camera integration and scene analysis

struct CameraDevice {
    std::string id;
    std::string name;
};

class VisionService {
public:
    VisionService(IAgentRuntime runtime);
    VisionConfig parseConfig(IAgentRuntime runtime);
    static std::future<VisionService> start(IAgentRuntime runtime);
    Promise< checkCameraTools();
    std::future<void> initialize();
    std::future<void> initializeScreenVision();
    std::future<void> initializeCameraVision();
    std::future<void> initializeAudioCapture();
    std::future<void> storeAudioTranscription(const std::string& text);
    void startProcessing();
    void startFrameProcessing();
    std::future<void> captureAndProcessFrame();
    std::future<VisionFrame> processFrameData(Buffer data);
    std::future<double> calculatePixelChange(VisionFrame frame1, VisionFrame frame2);
    std::future<void> updateSceneDescription(VisionFrame frame, double changePercentage);
    std::future<std::string> describeSceneWithVLM(const std::string& imageUrl);
    std::future<std::vector<DetectedObject>> detectMotionObjects(VisionFrame frame);
    std::vector<DetectedObject> mergeAdjacentObjects(const std::vector<DetectedObject>& objects);
    std::string classifyObjectBySize(double width, double height);
    std::future<std::vector<PersonInfo>> detectPeopleFromMotion(VisionFrame frame, const std::vector<DetectedObject>& objects);
    void startScreenProcessing();
    std::future<void> captureAndProcessScreen();
    std::future<TileAnalysis> analyzeTile(const std::any& tile);
    std::future<void> updateEnhancedSceneDescription();
    std::variant<Promise<VisionFrame, null>> getCurrentFrame();
    std::variant<Promise<SceneDescription, null>> getSceneDescription();
    std::variant<Promise<EnhancedSceneDescription, null>> getEnhancedSceneDescription();
    std::variant<Promise<ScreenCapture, null>> getScreenCapture();
    VisionMode getVisionMode();
    std::future<void> setVisionMode(VisionMode mode);
    void stopProcessing();
    std::optional<CameraInfo> getCameraInfo();
    bool isActive();
    double calculateBoxOverlap(BoundingBox box1, BoundingBox box2);
    std::string getDominantExpression(const std::any& expressions);
    EntityTracker getEntityTracker();
    FaceRecognition getFaceRecognition();
    std::future<void> stop();
    std::future<void> refresh();
    std::future<void> cleanupServices();
    std::future<void> reinitializeServices();
    std::variant<Promise<CameraDevice, null>> findCamera();
    std::future<std::vector<CameraInfo>> listCameras();
    CameraDevice createCameraDevice(CameraInfo info);
    std::variant<Promise<Buffer, null>> captureImage();
    std::future<void> processMediaStream(std::optional<std::any> data);
    std::variant<Promise<VisionFrame, null>> decodeVideoFrame(Uint8Array data, std::optional<std::string> encoding);
    std::future<void> processExternalFrame(const std::string& streamKey, VisionFrame frame);
    std::future<void> processAudioTranscriptionQueue();
    std::variant<Promise<string, null>> transcribeAudioWithWhisper(Buffer audioData);
    std::future<Buffer> pcmToWav(Buffer pcmData);
    std::unordered_map<std::string, std::any> getExternalStreams();
    void clearExternalStream(const std::string& streamKey);

private:
    VisionConfig visionConfig_;
    VisionModels visionModels_;
    FaceRecognition faceRecognition_;
    EntityTracker entityTracker_;
    ScreenCaptureService screenCapture_;
    Florence2Model florence2_;
    OCRService ocrService_;
};


} // namespace elizaos
