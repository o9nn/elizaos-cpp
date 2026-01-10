#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_TYPES_DASHBOARD_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_TYPES_DASHBOARD_H
#include "core.h"

class Partner;
class DashboardData;

class Partner : public object, public std::enable_shared_from_this<Partner> {
public:
    using std::enable_shared_from_this<Partner>::shared_from_this;
    string wallet;

    double trustScore;

    string image;
};

class DashboardData : public object, public std::enable_shared_from_this<DashboardData> {
public:
    using std::enable_shared_from_this<DashboardData>::shared_from_this;
    array<std::shared_ptr<Partner>> partners;
};

#endif
