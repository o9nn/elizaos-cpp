#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_UTILS_EXPERIENCEFORMATTER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_UTILS_EXPERIENCEFORMATTER_H
#include "core.h"
#include "../types.js.h"

std::string formatExperienceForDisplay(std::shared_ptr<Experience> experience);

std::string formatExperienceSummary(std::shared_ptr<Experience> experience);

std::string formatExperienceList(array<std::shared_ptr<Experience>> experiences);

std::string formatPatternSummary(object pattern);

std::shared_ptr<Map<std::string, array<std::shared_ptr<Experience>>>> groupExperiencesByDomain(array<std::shared_ptr<Experience>> experiences);

object getExperienceStats(array<std::shared_ptr<Experience>> experiences);

std::string getTypeEmoji(ExperienceType type);

std::string formatExperienceForRAG(std::shared_ptr<Experience> experience);

array<string> extractKeywords(std::shared_ptr<Experience> experience);

#endif
