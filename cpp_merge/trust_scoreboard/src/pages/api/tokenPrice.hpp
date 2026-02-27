#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_TRUST_SCOREBOARD_SRC_PAGES_API_TOKENPRICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_TRUST_SCOREBOARD_SRC_PAGES_API_TOKENPRICE_H
#include "core.hpp"
#include "next.hpp"

class TokenPrice;

extern string HELIUS_API;
extern array<string> TOKEN_ADDRESSES;
class TokenPrice : public object, public std::enable_shared_from_this<TokenPrice> {
public:
    using std::enable_shared_from_this<TokenPrice>::shared_from_this;
    string address;

    double usdPrice;
};

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res);

#endif
