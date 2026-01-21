#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_CHART_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_CHART_H
#include "core.h"
#include "drizzle-orm.h"
#include "./codex.h"
#include "./db.h"
#include "./redis.h"
#include "./util.h"
#include "./mcap.h"

typedef object CandlePrice;

class DexScreenerPair;
class DexScreenerResponse;
class PriceCandle;
class ChartResponse;
class PriceFeedInfo;
class Candle;

class DexScreenerPair : public object, public std::enable_shared_from_this<DexScreenerPair> {
public:
    using std::enable_shared_from_this<DexScreenerPair>::shared_from_this;
    std::string pairAddress;

    std::string priceUsd;

    object liquidity;
};

class DexScreenerResponse : public object, public std::enable_shared_from_this<DexScreenerResponse> {
public:
    using std::enable_shared_from_this<DexScreenerResponse>::shared_from_this;
    array<std::shared_ptr<DexScreenerPair>> pairs;
};

class PriceCandle : public object, public std::enable_shared_from_this<PriceCandle> {
public:
    using std::enable_shared_from_this<PriceCandle>::shared_from_this;
    double time;

    std::string open;

    std::string high;

    std::string low;

    std::string close;

    std::string volume;
};

class ChartResponse : public object, public std::enable_shared_from_this<ChartResponse> {
public:
    using std::enable_shared_from_this<ChartResponse>::shared_from_this;
    array<std::shared_ptr<PriceCandle>> priceCandles;
};

extern std::string DEV_TEST_TOKEN_ADDRESS;
class PriceFeedInfo : public object, public std::enable_shared_from_this<PriceFeedInfo> {
public:
    using std::enable_shared_from_this<PriceFeedInfo>::shared_from_this;
    double price;

    std::shared_ptr<Date> timestamp;

    double volume;

    double priceUsd;
};

std::any getLatestCandle(std::string tokenMint, std::any swap, std::any tokenInfo = undefined);

std::any fetchPriceChartData(double start, double end, double range, std::string tokenMint);

extern std::function<array<std::shared_ptr<CandlePrice>>(array<std::shared_ptr<PriceFeedInfo>>, double)> getCandleData;
std::shared_ptr<Promise<array<any>>> fetchLockedTokenChartData(std::string token, double start, double end, double range);

class Candle : public object, public std::enable_shared_from_this<Candle> {
public:
    using std::enable_shared_from_this<Candle>::shared_from_this;
    double open;

    double high;

    double low;

    double close;

    double volume;

    double time;
};

array<std::shared_ptr<Candle>> groupCandlesByRange(array<std::shared_ptr<Candle>> candles, double rangeMinutes);

std::shared_ptr<Candle> createCandleFromGroup(array<std::shared_ptr<Candle>> group, double rangeStart);

#endif
