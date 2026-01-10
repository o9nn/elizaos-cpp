#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_PAGES_API_PARTNERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_PAGES_API_PARTNERS_H
#include "core.h"
#include "next.h"

class Partner;

class Partner : public object, public std::enable_shared_from_this<Partner> {
public:
    using std::enable_shared_from_this<Partner>::shared_from_this;
    string owner;

    string displayAddress;

    double amount;

    double trustScore;
};

extern array<std::shared_ptr<Partner>> mockPartners;
extern std::function<double(double, double)> calculateTrustScore;
std::shared_ptr<Promise<array<std::shared_ptr<Partner>>>> getAllPartners();

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res);

#endif
