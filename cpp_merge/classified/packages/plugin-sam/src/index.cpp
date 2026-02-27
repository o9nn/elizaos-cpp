#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sam/src/index.h"

std::shared_ptr<Plugin> samPlugin = object{
    object::pair{std:("name"), std:("@elizaos/plugin-sam")}, 
    object::pair{std:("description"), std:("Retro text-to-speech using SAM Speech Synthesizer with hardware bridge integration")}, 
    object::pair{std:("actions"), array<any>{ sayAloudAction }}, 
    object::pair{std:("services"), array<SamTTSService>{ SamTTSService }}
};

void Main(void)
{
}

MAIN
