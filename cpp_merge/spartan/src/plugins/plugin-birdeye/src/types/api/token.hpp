#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_TYPES_API_TOKEN_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_TYPES_API_TOKEN_H
#include "core.hpp"
#include "./common.h"

class TokenTradesParams;
class TokenTradesResponse;
class TokenListParams;
class TokenListResponse;
class TokenSecurityParams;
class TokenSecurityResponse;
class TokenOverviewParams;
class TokenOverviewResponse;
class TokenCreationInfoParams;
class TokenCreationInfoResponse;
class TokenTrendingParams;
class TokenTrendingResponse;
class TokenListV2Params;
class TokenListV2Response;
class TokenMetadataMultiParams;
class TokenMetadataMultiResponse;
class TokenTradeDataMultiParams;
class TokenTradeDataMultiResponse;
class TokenMetadataSingleParams;
class TokenMetadataSingleResponse;
class TokenMarketDataParams;
class TokenMarketDataResponse;
class TokenTradeDataSingleParams;
class TokenTradeDataSingleResponse;
class TokenMarketStatsResponse;
class TokenHoldersParams;
class TokenHoldersResponse;
class MintBurnParams;
class MintBurnResponse;
class NewListingParams;
class NewListingResponse;
class TopTradersParams;
class TopTradersResponse;
class AllMarketsParams;
class AllMarketsResponse;
class TokenVolumeByOwnerResponse;

class TokenTradesParams : public object, public std::enable_shared_from_this<TokenTradesParams> {
public:
    using std::enable_shared_from_this<TokenTradesParams>::shared_from_this;
    string address;

    double limit;

    double offset;

    any type;
};

class TokenTradesResponse : public object, public std::enable_shared_from_this<TokenTradesResponse> {
public:
    using std::enable_shared_from_this<TokenTradesResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenListParams : public object, public std::enable_shared_from_this<TokenListParams> {
public:
    using std::enable_shared_from_this<TokenListParams>::shared_from_this;
    any sort_by;

    any sort_type;

    double offset;

    double limit;

    double min_liquidity;
};

class TokenListResponse : public object, public std::enable_shared_from_this<TokenListResponse> {
public:
    using std::enable_shared_from_this<TokenListResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenSecurityParams : public object, public std::enable_shared_from_this<TokenSecurityParams> {
public:
    using std::enable_shared_from_this<TokenSecurityParams>::shared_from_this;
    string address;
};

class TokenSecurityResponse : public object, public std::enable_shared_from_this<TokenSecurityResponse> {
public:
    using std::enable_shared_from_this<TokenSecurityResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenOverviewParams : public object, public std::enable_shared_from_this<TokenOverviewParams> {
public:
    using std::enable_shared_from_this<TokenOverviewParams>::shared_from_this;
    string address;
};

class TokenOverviewResponse : public object, public std::enable_shared_from_this<TokenOverviewResponse> {
public:
    using std::enable_shared_from_this<TokenOverviewResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenCreationInfoParams : public object, public std::enable_shared_from_this<TokenCreationInfoParams> {
public:
    using std::enable_shared_from_this<TokenCreationInfoParams>::shared_from_this;
    string address;
};

class TokenCreationInfoResponse : public object, public std::enable_shared_from_this<TokenCreationInfoResponse> {
public:
    using std::enable_shared_from_this<TokenCreationInfoResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenTrendingParams : public object, public std::enable_shared_from_this<TokenTrendingParams> {
public:
    using std::enable_shared_from_this<TokenTrendingParams>::shared_from_this;
    any sort_by;

    any sort_type;

    double offset;

    double limit;
};

class TokenTrendingResponse : public object, public std::enable_shared_from_this<TokenTrendingResponse> {
public:
    using std::enable_shared_from_this<TokenTrendingResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenListV2Params : public object, public std::enable_shared_from_this<TokenListV2Params> {
public:
    using std::enable_shared_from_this<TokenListV2Params>::shared_from_this;
    double offset;

    double limit;

    string sortBy;

    any sortOrder;
};

class TokenListV2Response : public object, public std::enable_shared_from_this<TokenListV2Response> {
public:
    using std::enable_shared_from_this<TokenListV2Response>::shared_from_this;
    boolean success;

    any data;
};

class TokenMetadataMultiParams : public object, public std::enable_shared_from_this<TokenMetadataMultiParams> {
public:
    using std::enable_shared_from_this<TokenMetadataMultiParams>::shared_from_this;
    string list_addresses;
};

class TokenMetadataMultiResponse : public object, public std::enable_shared_from_this<TokenMetadataMultiResponse> {
public:
    using std::enable_shared_from_this<TokenMetadataMultiResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenTradeDataMultiParams : public object, public std::enable_shared_from_this<TokenTradeDataMultiParams> {
public:
    using std::enable_shared_from_this<TokenTradeDataMultiParams>::shared_from_this;
    string list_addresses;
};

class TokenTradeDataMultiResponse : public object, public std::enable_shared_from_this<TokenTradeDataMultiResponse> {
public:
    using std::enable_shared_from_this<TokenTradeDataMultiResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenMetadataSingleParams : public object, public std::enable_shared_from_this<TokenMetadataSingleParams> {
public:
    using std::enable_shared_from_this<TokenMetadataSingleParams>::shared_from_this;
    string address;
};

class TokenMetadataSingleResponse : public object, public std::enable_shared_from_this<TokenMetadataSingleResponse> {
public:
    using std::enable_shared_from_this<TokenMetadataSingleResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenMarketDataParams : public object, public std::enable_shared_from_this<TokenMarketDataParams> {
public:
    using std::enable_shared_from_this<TokenMarketDataParams>::shared_from_this;
    string address;
};

class TokenMarketDataResponse : public object, public std::enable_shared_from_this<TokenMarketDataResponse> {
public:
    using std::enable_shared_from_this<TokenMarketDataResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenTradeDataSingleParams : public object, public std::enable_shared_from_this<TokenTradeDataSingleParams> {
public:
    using std::enable_shared_from_this<TokenTradeDataSingleParams>::shared_from_this;
    string address;
};

class TokenTradeDataSingleResponse : public object, public std::enable_shared_from_this<TokenTradeDataSingleResponse> {
public:
    using std::enable_shared_from_this<TokenTradeDataSingleResponse>::shared_from_this;
    boolean success;

    std::shared_ptr<TokenTradeData> data;
};

class TokenMarketStatsResponse : public object, public std::enable_shared_from_this<TokenMarketStatsResponse> {
public:
    using std::enable_shared_from_this<TokenMarketStatsResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenHoldersParams : public object, public std::enable_shared_from_this<TokenHoldersParams> {
public:
    using std::enable_shared_from_this<TokenHoldersParams>::shared_from_this;
    string address;

    double offset;

    double limit;
};

class TokenHoldersResponse : public object, public std::enable_shared_from_this<TokenHoldersResponse> {
public:
    using std::enable_shared_from_this<TokenHoldersResponse>::shared_from_this;
    boolean success;

    object data;
};

class MintBurnParams : public object, public std::enable_shared_from_this<MintBurnParams> {
public:
    using std::enable_shared_from_this<MintBurnParams>::shared_from_this;
    string address;

    string sort_by;

    any sort_type;

    any type;

    double after_time;

    double before_time;

    double offset;

    double limit;
};

class MintBurnResponse : public object, public std::enable_shared_from_this<MintBurnResponse> {
public:
    using std::enable_shared_from_this<MintBurnResponse>::shared_from_this;
    boolean success;

    object data;
};

class NewListingParams : public object, public std::enable_shared_from_this<NewListingParams> {
public:
    using std::enable_shared_from_this<NewListingParams>::shared_from_this;
    double time_to;

    boolean meme_platform_enabled;

    double limit;
};

class NewListingResponse : public object, public std::enable_shared_from_this<NewListingResponse> {
public:
    using std::enable_shared_from_this<NewListingResponse>::shared_from_this;
    boolean success;

    object data;
};

class TopTradersParams : public object, public std::enable_shared_from_this<TopTradersParams> {
public:
    using std::enable_shared_from_this<TopTradersParams>::shared_from_this;
    string address;

    TimeInterval time_frame;

    any sort_type;

    any sort_by;

    double offset;

    double limit;
};

class TopTradersResponse : public object, public std::enable_shared_from_this<TopTradersResponse> {
public:
    using std::enable_shared_from_this<TopTradersResponse>::shared_from_this;
    boolean success;

    object data;
};

class AllMarketsParams : public object, public std::enable_shared_from_this<AllMarketsParams> {
public:
    using std::enable_shared_from_this<AllMarketsParams>::shared_from_this;
    string address;

    TimeInterval time_frame;

    any sort_type;

    any sort_by;

    double offset;

    double limit;
};

class AllMarketsResponse : public object, public std::enable_shared_from_this<AllMarketsResponse> {
public:
    using std::enable_shared_from_this<AllMarketsResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenVolumeByOwnerResponse : public object, public std::enable_shared_from_this<TokenVolumeByOwnerResponse> {
public:
    using std::enable_shared_from_this<TokenVolumeByOwnerResponse>::shared_from_this;
    boolean success;

    object data;
};

#endif
