#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_HOOKS_USEDASHBOARD_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_HOOKS_USEDASHBOARD_H
#include "core.h"
#include "react.h"
#include "../utils/axios.h"

class Partner;
class DashboardData;

extern std::function<std::shared_ptr<Promise<any>>()> fetchHighestRankedUsers;
class Partner : public object, public std::enable_shared_from_this<Partner> {
public:
    using std::enable_shared_from_this<Partner>::shared_from_this;
    double trustScore;

    string avatarUrl;

    double rank;

    string id;

    string name;
};

class DashboardData : public object, public std::enable_shared_from_this<DashboardData> {
public:
    using std::enable_shared_from_this<DashboardData>::shared_from_this;
    array<std::shared_ptr<Partner>> partners;
};

extern std::function<object()> useDashboard;
#endif
