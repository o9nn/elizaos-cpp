#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/index.h"

std::shared_ptr<Plugin> experiencePlugin = object{
    object::pair{std::string("name"), std::string("experience")}, 
    object::pair{std::string("description"), std::string("Self-learning experience system that records experiences and learns from agent interactions")}, 
    object::pair{std::string("services"), array<ExperienceService>{ ExperienceService }}, 
    object::pair{std::string("providers"), array<any>{ experienceRAGProvider, recentExperiencesProvider }}, 
    object::pair{std::string("evaluators"), array<any>{ experienceEvaluator }}, 
    object::pair{std::string("init"), [=](auto config, auto runtime) mutable
    {
        logger->info(std::string("[ExperiencePlugin] Initializing self-learning experience system"));
        auto maxExperiences = OR((config->maxExperiences), (10000));
        auto autoRecordThreshold = OR((config->autoRecordThreshold), (0.7));
        logger->info(std::string("[ExperiencePlugin] Configuration read:\
    - Max experiences: ") + maxExperiences + std::string("\
    - Auto-record threshold: ") + autoRecordThreshold + string_empty);
    }
    }
};

void Main(void)
{
}

MAIN
