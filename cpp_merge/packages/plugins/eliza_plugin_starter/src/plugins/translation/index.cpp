#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-plugin-starter/src/plugins/translation/index.h"

std::shared_ptr<Plugin> translationPlugin = object{
    object::pair{std::string("name"), std::string("translation")}, 
    object::pair{std::string("description"), std::string("A plugin for translating text between different languages")}, 
    object::pair{std::string("actions"), array<any>{ getTranslationAction }}, 
    object::pair{std::string("evaluators"), array<any>{ translationEvaluator }}, 
    object::pair{std::string("providers"), array<any>{ translationProvider }}, 
    object::pair{std::string("services"), array<any>{ translationService }}
};
std::function<void(any)> initializeTranslationPlugin = [=](auto config) mutable
{
    if (config->provider) {
        initializeTranslationConfig(config->provider);
    }
};

void Main(void)
{
}

MAIN
