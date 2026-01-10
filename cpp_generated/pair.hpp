#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_API_PAIR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_API_PAIR_H
#include "core.h"
#include "./common.h"

class PairTradesParams;
class PairTradesResponse;
class OHLCVPairParams;
class OHLCVPairResponse;
class PairOverviewMultiParams;
class PairOverviewSingleParams;
class PairOverviewData;
class PairOverviewSingleResponse;
class PairOverviewMultiResponse;

class PairTradesParams : public object, public std::enable_shared_from_this<PairTradesParams> {
public:
    using std::enable_shared_from_this<PairTradesParams>::shared_from_this;
    string pair;

    double limit;

    double offset;
};

class PairTradesResponse : public object, public std::enable_shared_from_this<PairTradesResponse> {
public:
    using std::enable_shared_from_this<PairTradesResponse>::shared_from_this;
    boolean success;

    object data;
};

class OHLCVPairParams : public object, public std::enable_shared_from_this<OHLCVPairParams> {
public:
    using std::enable_shared_from_this<OHLCVPairParams>::shared_from_this;
    string address;

    TimeInterval type;

    double time_from;

    double time_to;
};

class OHLCVPairResponse : public object, public std::enable_shared_from_this<OHLCVPairResponse> {
public:
    using std::enable_shared_from_this<OHLCVPairResponse>::shared_from_this;
    boolean success;

    object data;
};

class PairOverviewMultiParams : public object, public std::enable_shared_from_this<PairOverviewMultiParams> {
public:
    using std::enable_shared_from_this<PairOverviewMultiParams>::shared_from_this;
    string list_address;

    double before_time;
};

class PairOverviewSingleParams : public object, public std::enable_shared_from_this<PairOverviewSingleParams> {
public:
    using std::enable_shared_from_this<PairOverviewSingleParams>::shared_from_this;
    string address;
};

class PairOverviewData : public object, public std::enable_shared_from_this<PairOverviewData> {
public:
    using std::enable_shared_from_this<PairOverviewData>::shared_from_this;
    string address;

    string name;

    object base;

    object quote;

    string created_at;

    string source;

    double liquidity;

    any liquidity_change_percentage_24h;

    double price;

    double volume_24h;

    any volume_24h_change_percentage_24h;

    double trade_24h;

    double trade_24h_change_percent;

    double unique_wallet_24h;

    any unique_wallet_24h_change_percent;

    double trade_30m;

    double trade_1h;

    double trade_2h;

    double trade_4h;

    double trade_8h;

    double trade_12h;

    double trade_30m_change_percent;

    double trade_1h_change_percent;

    double trade_2h_change_percent;

    double trade_4h_change_percent;

    double trade_8h_change_percent;

    double trade_12h_change_percent;

    double volume_30m;

    double volume_1h;

    double volume_2h;

    double volume_4h;

    double volume_8h;

    double volume_12h;

    double volume_30m_quote;

    double volume_1h_quote;

    double volume_2h_quote;

    double volume_4h_quote;

    double volume_8h_quote;

    double volume_12h_quote;

    double volume_30m_base;

    double volume_1h_base;

    double volume_2h_base;

    double volume_4h_base;

    double volume_8h_base;

    double volume_12h_base;
};

class PairOverviewSingleResponse : public object, public std::enable_shared_from_this<PairOverviewSingleResponse> {
public:
    using std::enable_shared_from_this<PairOverviewSingleResponse>::shared_from_this;
    boolean success;

    object data;
};

class PairOverviewMultiResponse : public object, public std::enable_shared_from_this<PairOverviewMultiResponse> {
public:
    using std::enable_shared_from_this<PairOverviewMultiResponse>::shared_from_this;
    boolean success;

    object data;
};

#endif
