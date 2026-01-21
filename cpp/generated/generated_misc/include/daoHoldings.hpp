#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_PAGES_API_DAOHOLDINGS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_TRUST_SCOREBOARD_SRC_PAGES_API_DAOHOLDINGS_H
#include "core.h"
#include "next.h"

class TokenInfo;
class HeliusResponse;
class DAOHolding;

class TokenInfo : public object, public std::enable_shared_from_this<TokenInfo> {
public:
    using std::enable_shared_from_this<TokenInfo>::shared_from_this;
    std::string name;

    std::string symbol;

    double balance;

    double decimals;

    object price_info;
};

class HeliusResponse : public object, public std::enable_shared_from_this<HeliusResponse> {
public:
    using std::enable_shared_from_this<HeliusResponse>::shared_from_this;
    object result;
};

class DAOHolding : public object, public std::enable_shared_from_this<DAOHolding> {
public:
    using std::enable_shared_from_this<DAOHolding>::shared_from_this;
    double rank;

    std::string name;

    std::string holdings;

    std::string value;

    std::string percentage;

    std::string imageUrl;
};

std::any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res);

std::string formatCurrency(double amount);

std::string calculatePercentage(double amount, double total);

std::string formatTokenAmount(double amount, double decimals = 18);

#endif
