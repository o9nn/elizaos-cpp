#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_CDP_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_CDP_H
#include "core.h"

class WalletRequest;
class WalletResponse;
class TopAndTrendingTokensRequest;
class TopAndTrendingTokensResponse;

class WalletRequest : public object, public std::enable_shared_from_this<WalletRequest> {
public:
    using std::enable_shared_from_this<WalletRequest>::shared_from_this;
    string name;
};

class WalletResponse : public object, public std::enable_shared_from_this<WalletResponse> {
public:
    using std::enable_shared_from_this<WalletResponse>::shared_from_this;
    string address;

    string accountName;
};

class TopAndTrendingTokensRequest : public object, public std::enable_shared_from_this<TopAndTrendingTokensRequest> {
public:
    using std::enable_shared_from_this<TopAndTrendingTokensRequest>::shared_from_this;
    string chain;

    double limit;
};

class TopAndTrendingTokensResponse : public object, public std::enable_shared_from_this<TopAndTrendingTokensResponse> {
public:
    using std::enable_shared_from_this<TopAndTrendingTokensResponse>::shared_from_this;
    array<any> topTokens;

    array<any> trendingTokens;
};

#endif
