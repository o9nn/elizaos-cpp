#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class VisionCaptureLogTestSuite {
public:
    void if(auto !visionService);
    void if(auto imageBuffer);
    void if(auto imageBase64);
    void if(auto scene);
    void catch(auto error);
    void if(auto waitTime > 0);
};


} // namespace elizaos
