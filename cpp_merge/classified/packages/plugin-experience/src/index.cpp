#include "index.hpp"

std::shared_ptr<Plugin> experiencePlugin = object{
    object::pair{std:("name"), std:("experience")}, 
    object::pair{std:("description"), std:("Self-learning experience system that records experiences and learns from agent interactions")}, 
    object::pair{std:("services"), array<ExperienceService>{ ExperienceService }}, 
    object::pair{std:("providers"), array<any>{ experienceProvider }}, 
    object::pair{std:("evaluators"), array<any>{ experienceEvaluator }}, 
    object::pair{std:("init"), [=](auto config, auto runtime) mutable
    {
        logger->info(std:("[ExperiencePlugin] Initializing self-learning experience system"));
        auto maxExperiences = OR((config->maxExperiences), (10000));
        auto autoRecordThreshold = OR((config->autoRecordThreshold), (0.7));
        logger->info(std:("[ExperiencePlugin] Configuration read:\
    - Max experiences: ") + maxExperiences + std:("\
    - Auto-record threshold: ") + autoRecordThreshold + string_empty);
    }
    }
};

void Main(void)
{
}

MAIN
