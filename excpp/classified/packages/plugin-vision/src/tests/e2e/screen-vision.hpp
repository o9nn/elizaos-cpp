#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..action.hpp"
#include "..service.hpp"
#include "..types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ScreenVisionE2ETestSuite {
public:
    void if(auto !visionService);
    void if(auto screenInfo);
    void if(auto !visionService);
    void if(auto !screenCapture);
    void if(auto !firstTile);
    void if(auto !visionService);
    void if(auto !enhancedScene || !enhancedScene.screenAnalysis);
    void if(auto screenAnalysis.activeTile);
    void if(auto screenAnalysis.fullScreenOCR);
    void if(auto screenAnalysis.uiElements && screenAnalysis.uiElements.length > 0);
    void if(auto !visionService);
    void for(auto const mode of modes);
    void if(auto !callbackCalled);
    void if(auto !visionService);
    void if(auto !hasCamera && !hasScreen);
    void if(auto enhancedScene);
    void if(auto !visionService);
};


} // namespace elizaos
