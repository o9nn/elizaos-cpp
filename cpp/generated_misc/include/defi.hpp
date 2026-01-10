#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_API_DEFI_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_API_DEFI_H
#include "core.h"
#include "./common.h"

class DefiNetworksResponse;
class DefiPriceParams;
class DefiPriceResponse;
class DefiMultiPriceParams;
class DefiMultiPriceResponse;
class DefiMultiPriceParamsPOST;
class DefiHistoryPriceParams;
class DefiHistoryPriceResponse;
class HistoricalPriceUnixParams;
class HistoricalPriceUnixResponse;
class OHLCVParams;
class OHLCVResponse;
class PriceVolumeParams;
class PriceVolumeResponse;
class MultiPriceVolumeParams;
class MultiPriceVolumeResponse;
class BaseQuoteParams;
class BaseQuoteResponse;
class DefiTradesTokenParams;
class DefiTradesTokenInfo;
class DefiTradesTokenResponse;

class DefiNetworksResponse : public object, public std::enable_shared_from_this<DefiNetworksResponse> {
public:
    using std::enable_shared_from_this<DefiNetworksResponse>::shared_from_this;
    boolean success;

    object data;
};

class DefiPriceParams : public object, public std::enable_shared_from_this<DefiPriceParams> {
public:
    using std::enable_shared_from_this<DefiPriceParams>::shared_from_this;
    string address;

    double check_liquidity;

    boolean include_liquidity;
};

class DefiPriceResponse : public object, public std::enable_shared_from_this<DefiPriceResponse> {
public:
    using std::enable_shared_from_this<DefiPriceResponse>::shared_from_this;
    boolean success;

    object data;
};

class DefiMultiPriceParams : public object, public std::enable_shared_from_this<DefiMultiPriceParams> {
public:
    using std::enable_shared_from_this<DefiMultiPriceParams>::shared_from_this;
    string list_address;

    double check_liquidity;

    boolean include_liquidity;
};

class DefiMultiPriceResponse : public object, public std::enable_shared_from_this<DefiMultiPriceResponse> {
public:
    using std::enable_shared_from_this<DefiMultiPriceResponse>::shared_from_this;
    boolean success;

    object data;
};

class DefiMultiPriceParamsPOST : public object, public std::enable_shared_from_this<DefiMultiPriceParamsPOST> {
public:
    using std::enable_shared_from_this<DefiMultiPriceParamsPOST>::shared_from_this;
    double check_liquidity;

    boolean include_liquidity;

    string list_address;
};

class DefiHistoryPriceParams : public object, public std::enable_shared_from_this<DefiHistoryPriceParams> {
public:
    using std::enable_shared_from_this<DefiHistoryPriceParams>::shared_from_this;
    string address;

    any address_type;

    TimeInterval type;

    double time_from;

    double time_to;
};

class DefiHistoryPriceResponse : public object, public std::enable_shared_from_this<DefiHistoryPriceResponse> {
public:
    using std::enable_shared_from_this<DefiHistoryPriceResponse>::shared_from_this;
    boolean success;

    object data;
};

class HistoricalPriceUnixParams : public object, public std::enable_shared_from_this<HistoricalPriceUnixParams> {
public:
    using std::enable_shared_from_this<HistoricalPriceUnixParams>::shared_from_this;
    string address;

    double unixtime;
};

class HistoricalPriceUnixResponse : public object, public std::enable_shared_from_this<HistoricalPriceUnixResponse> {
public:
    using std::enable_shared_from_this<HistoricalPriceUnixResponse>::shared_from_this;
    boolean success;

    object data;
};

class OHLCVParams : public object, public std::enable_shared_from_this<OHLCVParams> {
public:
    using std::enable_shared_from_this<OHLCVParams>::shared_from_this;
    string address;

    TimeInterval type;

    double time_from;

    double time_to;
};

class OHLCVResponse : public object, public std::enable_shared_from_this<OHLCVResponse> {
public:
    using std::enable_shared_from_this<OHLCVResponse>::shared_from_this;
    boolean success;

    object data;
};

class PriceVolumeParams : public object, public std::enable_shared_from_this<PriceVolumeParams> {
public:
    using std::enable_shared_from_this<PriceVolumeParams>::shared_from_this;
    string address;

    TimeInterval type;
};

class PriceVolumeResponse : public object, public std::enable_shared_from_this<PriceVolumeResponse> {
public:
    using std::enable_shared_from_this<PriceVolumeResponse>::shared_from_this;
    boolean success;

    object data;
};

class MultiPriceVolumeParams : public object, public std::enable_shared_from_this<MultiPriceVolumeParams> {
public:
    using std::enable_shared_from_this<MultiPriceVolumeParams>::shared_from_this;
    string list_address;

    TimeInterval type;
};

class MultiPriceVolumeResponse : public object, public std::enable_shared_from_this<MultiPriceVolumeResponse> {
public:
    using std::enable_shared_from_this<MultiPriceVolumeResponse>::shared_from_this;
    boolean success;

    object data;
};

class BaseQuoteParams : public object, public std::enable_shared_from_this<BaseQuoteParams> {
public:
    using std::enable_shared_from_this<BaseQuoteParams>::shared_from_this;
    string base_address;

    string quote_address;

    TimeInterval type;

    double time_from;

    double time_to;
};

class BaseQuoteResponse : public object, public std::enable_shared_from_this<BaseQuoteResponse> {
public:
    using std::enable_shared_from_this<BaseQuoteResponse>::shared_from_this;
    boolean success;

    object data;
};

class DefiTradesTokenParams : public object, public std::enable_shared_from_this<DefiTradesTokenParams> {
public:
    using std::enable_shared_from_this<DefiTradesTokenParams>::shared_from_this;
    string address;

    double limit;

    double offset;

    any tx_type;

    any sort_type;

    double before_time;

    double after_time;
};

class DefiTradesTokenInfo : public object, public std::enable_shared_from_this<DefiTradesTokenInfo> {
public:
    using std::enable_shared_from_this<DefiTradesTokenInfo>::shared_from_this;
    string symbol;

    double decimals;

    string address;

    double amount;

    double uiAmount;

    any price;

    any nearestPrice;

    double changeAmount;

    double uiChangeAmount;

    any feeInfo;
};

class DefiTradesTokenResponse : public object, public std::enable_shared_from_this<DefiTradesTokenResponse> {
public:
    using std::enable_shared_from_this<DefiTradesTokenResponse>::shared_from_this;
    boolean success;

    object data;
};

#endif
