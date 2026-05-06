#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_COINMARKETCAP_SRC_ACTIONS_GETPRICE_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_COINMARKETCAP_SRC_ACTIONS_GETPRICE_TYPES_H
#include "core.hpp"
// External dependency removed

class GetPriceContent;
class PriceData;
class ApiResponse;

class GetPriceContent : public Content, public std::enable_shared_from_this<GetPriceContent> {
public:
    using std::enable_shared_from_this<GetPriceContent>::shared_from_this;
    string symbol;

    string currency;
};

class PriceData : public object, public std::enable_shared_from_this<PriceData> {
public:
    using std::enable_shared_from_this<PriceData>::shared_from_this;
    double price;

    double marketCap;

    double volume24h;

    double percentChange24h;
};

class ApiResponse : public object, public std::enable_shared_from_this<ApiResponse> {
public:
    using std::enable_shared_from_this<ApiResponse>::shared_from_this;
    object data;
};

#endif
