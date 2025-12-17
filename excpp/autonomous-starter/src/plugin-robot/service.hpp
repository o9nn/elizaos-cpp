#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// @ts-ignore - Mocked module, will be replaced by actual implementation

class RobotService {
public:
    RobotService(IAgentRuntime runtime);
    static std::future<Service> start(IAgentRuntime runtime);
    static std::future<void> stop(IAgentRuntime runtime);
    std::future<Buffer> captureScreen();
    Buffer createMockPNG(double width, double height);
    Buffer downscaleImage(Buffer imageBuffer, double maxSize = 1024);
    double calculatePixelDifference(Buffer buffer1, Buffer buffer2);
     shouldTriggerAIProcessing(Buffer currentScreenshot);
    std::future<std::string> performLocalOCR(Buffer image);
    bool isValidImageBuffer(Buffer buffer);
    std::future<std::string> performAIOCR(Buffer image);
    std::future<std::string> describeImage(Buffer image);
    std::future<std::vector<ScreenObject>> detectObjects(Buffer image);
    std::string formatRelativeTime(double timestamp);
    void updateDescriptionHistory(const std::string& description, double timestamp);
    std::future<void> updateContextInBackground();
    std::future<void> performUpdate();
    std::future<void> updateContext();
    std::future<ScreenContext> getContext();
    void moveMouse(double x, double y);
    void typeText(const std::string& text);
    void setChangeDetectionThreshold(double threshold);
    void enableChangeDetection(bool enabled);
};


} // namespace elizaos
