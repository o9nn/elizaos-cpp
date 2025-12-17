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
    std::future<std::vector<uint8_t>> captureScreen();
    std::vector<uint8_t> createMockPNG(double width, double height);
    std::vector<uint8_t> downscaleImage(const std::vector<uint8_t>& imageBuffer, double maxSize = 1024);
    double calculatePixelDifference(const std::vector<uint8_t>& buffer1, const std::vector<uint8_t>& buffer2);
     shouldTriggerAIProcessing(const std::vector<uint8_t>& currentScreenshot);
    std::future<std::string> performLocalOCR(const std::vector<uint8_t>& image);
    bool isValidImageBuffer(const std::vector<uint8_t>& buffer);
    std::future<std::string> performAIOCR(const std::vector<uint8_t>& image);
    std::future<std::string> describeImage(const std::vector<uint8_t>& image);
    std::future<std::vector<ScreenObject>> detectObjects(const std::vector<uint8_t>& image);
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
