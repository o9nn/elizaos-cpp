#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-DEFILLAMA_SRC_UTILS_ACTION-HELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-DEFILLAMA_SRC_UTILS_ACTION-HELPERS_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"

template <typename P0>
any parsePositiveInteger(P0 value);

template <typename T>
array<T> limitSeries(array<T> series, double limit);

template <typename T>
array<T> downsampleSeries(array<T> series, double maxPoints = 50);

template <typename T>
any calculateTvlSummary(array<T> series);

template <typename P3>
std::shared_ptr<Promise<std::shared_ptr<ActionResult>>> respondWithError(any callback, string messageText, string errorCode, P3 details = undefined);

extern std::shared_ptr<RegExp> CHAIN_NAME_PATTERN;
any sanitizeChainName(any value);

extern std::shared_ptr<RegExp> FILTER_PATTERN;
any sanitizeFilterSegment(any value);

template <typename P0>
any parsePositiveInteger(P0 value)
{
    if (AND((AND((type_of(value) == std::string("number")), (Number->isInteger(value)))), (value > 0))) {
        return value;
    }
    if (AND((type_of(value) == std::string("string")), (value->trim()))) {
        auto parsed = Number(value);
        if (AND((Number->isInteger(parsed)), (parsed > 0))) {
            return parsed;
        }
    }
    return undefined;
};


template <typename T>
array<T> limitSeries(array<T> series, double limit)
{
    if (OR((!limit), (series->get_length() <= limit))) {
        return series;
    }
    return series->slice(series->get_length() - limit);
};


template <typename T>
array<T> downsampleSeries(array<T> series, double maxPoints)
{
    if (series->get_length() <= maxPoints) {
        return series;
    }
    auto step = Math->ceil(series->get_length() / maxPoints);
    auto downsampled = array<T>();
    downsampled->push(const_(series)[0]);
    for (auto i = step; i < series->get_length() - 1; i += step)
    {
        downsampled->push(const_(series)[i]);
    }
    downsampled->push(const_(series)[series->get_length() - 1]);
    return downsampled;
};


template <typename T>
any calculateTvlSummary(array<T> series)
{
    if (series->get_length() == 0) {
        return nullptr;
    }
    auto values = series->map([=](auto p) mutable
    {
        return p->totalLiquidityUsd;
    }
    );
    auto current = const_(values)[values->get_length() - 1];
    auto first = const_(values)[0];
    auto min = Math->min(const_(values)[0]);
    auto max = Math->max(const_(values)[0]);
    auto athValue = -Infinity;
    auto athIndex = 0;
    for (auto i = 0; i < series->get_length(); i++)
    {
        if (const_(series)[i]->totalLiquidityUsd > athValue) {
            athValue = const_(series)[i]->totalLiquidityUsd;
            athIndex = i;
        }
    }
    auto athDate = const_(series)[athIndex]->date;
    auto lastDate = const_(series)[series->get_length() - 1]->date;
    auto athDaysAgo = Math->floor((lastDate - athDate) / 86400);
    auto fromAth = current - athValue;
    auto fromAthPercent = (athValue != 0) ? any((fromAth / athValue) * 100) : any(0);
    auto average = values->reduce([=](auto sum, auto val) mutable
    {
        return sum + val;
    }
    , 0) / values->get_length();
    auto change = current - first;
    auto changePercent = (first != 0) ? any((change / first) * 100) : any(0);
    return object{
        object::pair{std::string("current"), Math->round(current)}, 
        object::pair{std::string("min"), Math->round(min)}, 
        object::pair{std::string("max"), Math->round(max)}, 
        object::pair{std::string("ath"), Math->round(athValue)}, 
        object::pair{std::string("athDate"), std::string("athDate")}, 
        object::pair{std::string("athDaysAgo"), std::string("athDaysAgo")}, 
        object::pair{std::string("fromAth"), Math->round(fromAth)}, 
        object::pair{std::string("fromAthPercent"), Math->round(fromAthPercent * 100) / 100}, 
        object::pair{std::string("average"), Math->round(average)}, 
        object::pair{std::string("change"), Math->round(change)}, 
        object::pair{std::string("changePercent"), Math->round(changePercent * 100) / 100}, 
        object::pair{std::string("dataPoints"), series->get_length()}, 
        object::pair{std::string("firstDate"), const_(series)[0]->date}, 
        object::pair{std::string("lastDate"), const_(series)[series->get_length() - 1]->date}
    };
};


template <typename P3>
std::shared_ptr<Promise<std::shared_ptr<ActionResult>>> respondWithError(any callback, string messageText, string errorCode, P3 details)
{
    if (callback) {
        std::async([=]() { callback(object{
            object::pair{std::string("text"), messageText}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("error"), errorCode}, 
                object::pair{std::string("details"), std::string("details")}
            }}
        }); });
    }
    return object{
        object::pair{std::string("text"), messageText}, 
        object::pair{std::string("success"), false}, 
        object::pair{std::string("error"), errorCode}, 
        object::pair{std::string("data"), details}
    };
};


#endif
