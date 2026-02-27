#include "index.hpp"

std::shared_ptr<Plugin> translationPlugin = object{
    object::pair{std:("name"), std:("translation")}, 
    object::pair{std:("description"), std:("A plugin for translating text between different languages")}, 
    object::pair{std:("actions"), array<any>{ getTranslationAction }}, 
    object::pair{std:("evaluators"), array<any>{ translationEvaluator }}, 
    object::pair{std:("providers"), array<any>{ translationProvider }}, 
    object::pair{std:("services"), array<any>{ translationService }}
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
