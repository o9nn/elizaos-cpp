#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_EXPERIENCE_UTILS_EXPERIENCEFORMATTER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_EXPERIENCE_UTILS_EXPERIENCEFORMATTER_H
#include "core.hpp"
#include "../types.js.h"

string formatExperienceForDisplay(std::shared_ptr<Experience> experience);

string formatExperienceSummary(std::shared_ptr<Experience> experience);

string formatExperienceList(array<std::shared_ptr<Experience>> experiences);

string formatPatternSummary(object pattern);

std::shared_ptr<Map<string, array<std::shared_ptr<Experience>>>> groupExperiencesByDomain(array<std::shared_ptr<Experience>> experiences);

object getExperienceStats(array<std::shared_ptr<Experience>> experiences);

string getTypeEmoji(ExperienceType type);

string formatExperienceForRAG(std::shared_ptr<Experience> experience);

array<string> extractKeywords(std::shared_ptr<Experience> experience);

#endif
