#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/index.h"

std::shared_ptr<Plugin> visionPlugin = object{
    object::pair{std::string("name"), std::string("vision")}, 
    object::pair{std::string("description"), std::string("Provides visual perception through camera integration, scene analysis, and audio capture")}, 
    object::pair{std::string("services"), array<VisionService>{ VisionService }}, 
    object::pair{std::string("providers"), array<any>{ visionProvider }}, 
    object::pair{std::string("actions"), array<any>{ describeSceneAction, captureImageAction, setVisionModeAction, nameEntityAction, identifyPersonAction, trackEntityAction, transcribeAudioAction, toggleStreamingAudioAction }}, 
    object::pair{std::string("tests"), testSuites}, 
    object::pair{std::string("init"), [=](auto _config, auto _runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
