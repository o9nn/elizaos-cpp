#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_UTILS_DATETIME_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_UTILS_DATETIME_H
#include "core.hpp"
#include <string>

boolean isAvailableNow(array<string> workDays, object workHours, string timeZone);

std::shared_ptr<Date> calculateNextCheckIn(array<string> workDays, object workHours, string timeZone, double frequencyHours = 24);

string formatDate(std::shared_ptr<Date> date, string timeZone = std::nullopt);

template <typename RET>
RET getProjectStatus(double completionPercentage, double elapsedTimePercentage);

template <typename RET>
RET getProjectStatus(double completionPercentage, double elapsedTimePercentage)
{
    auto difference = completionPercentage - elapsedTimePercentage;
    if (difference >= -10) {
        return std::string("ON_TRACK");
    } else if (difference >= -20) {
        return std::string("AT_RISK");
    } else {
        return std::string("DELAYED");
    }
};


#endif
