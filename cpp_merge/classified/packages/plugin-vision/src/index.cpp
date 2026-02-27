#include "index.hpp"

std::shared_ptr<Plugin> visionPlugin = object{
    object::pair{std:("name"), std:("vision")}, 
    object::pair{std:("description"), std:("Provides visual perception through camera integration, scene analysis, and audio capture")}, 
    object::pair{std:("services"), array<VisionService>{ VisionService }}, 
    object::pair{std:("providers"), array<any>{ visionProvider }}, 
    object::pair{std:("actions"), array<any>{ describeSceneAction, captureImageAction, setVisionModeAction, nameEntityAction, identifyPersonAction, trackEntityAction, transcribeAudioAction, toggleStreamingAudioAction }}, 
    object::pair{std:("tests"), testSuites}, 
    object::pair{std:("init"), [=](auto _config, auto _runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
