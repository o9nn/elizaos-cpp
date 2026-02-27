#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sam/src/index.h"

std::shared_ptr<Plugin> samPlugin = object{
    object::pair{std::string("name"), std::string("@elizaos/plugin-sam")}, 
    object::pair{std::string("description"), std::string("Retro text-to-speech using SAM Speech Synthesizer with hardware bridge integration")}, 
    object::pair{std::string("actions"), array<any>{ sayAloudAction }}, 
    object::pair{std::string("services"), array<SamTTSService>{ SamTTSService }}
};

void Main(void)
{
}

MAIN
