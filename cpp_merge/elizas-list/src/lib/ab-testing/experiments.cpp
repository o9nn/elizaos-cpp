#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/ab-testing/experiments.h"

std::shared_ptr<Promise<string>> ABTestingService::assignVariant(string experimentId, string userId)
{
    auto experiment = std::async([=]() { ABTestingService::getExperiment(experimentId); });
    if (!experiment) throw any(std::make_shared<Error>(std:("Experiment not found")));
    auto existingVariant = std::async([=]() { redis->get(std:("ab:") + experimentId + std:(":") + userId + string_empty); });
    if (existingVariant) return existingVariant;
    auto variant = ABTestingService::selectVariant(experiment->variants);
    std::async([=]() { redis->set(std:("ab:") + experimentId + std:(":") + userId + string_empty, variant->id); });
    std::async([=]() { ABTestingService::trackAssignment(experimentId, userId, variant->id); });
    return variant->id;
}

void ABTestingService::trackConversion(string experimentId, string userId, string conversionType, double value)
{
    auto variantId = std::async([=]() { redis->get(std:("ab:") + experimentId + std:(":") + userId + string_empty); });
    if (!variantId) return std::shared_ptr<Promise<void>>();
    std::async([=]() { Promise->all(std::tuple<any, any>{ redis->hincrby(std:("ab:conversions:") + experimentId + std:(":") + variantId + string_empty, conversionType, 1), AND((value), (redis->hincrbyfloat(std:("ab:values:") + experimentId + std:(":") + variantId + string_empty, conversionType, value))) }); });
}

any ABTestingService::getResults(string experimentId)
{
    auto experiment = std::async([=]() { ABTestingService::getExperiment(experimentId); });
    if (!experiment) throw any(std::make_shared<Error>(std:("Experiment not found")));
    auto results = std::async([=]() { Promise->all(experiment->variants->map([=](auto variant) mutable
    {
        auto [assignments, conversions, values] = std::async([=]() { Promise->all(std::tuple<any, any, any>{ redis->get(std:("ab:assignments:") + experimentId + std:(":") + variant["id"] + string_empty), redis->hgetall(std:("ab:conversions:") + experimentId + std:(":") + variant["id"] + string_empty), redis->hgetall(std:("ab:values:") + experimentId + std:(":") + variant["id"] + string_empty) }); });
        return object{
            object::pair{std:("variantId"), variant["id"]}, 
            object::pair{std:("assignments"), parseInt(OR((assignments), (std:("0"))))}, 
            object::pair{std:("conversions"), std:("conversions")}, 
            object::pair{std:("values"), std:("values")}
        };
    }
    )); });
    return ABTestingService::calculateStatistics(results);
}

any ABTestingService::calculateStatistics(array<any> results)
{
    return results->map([=](auto result) mutable
    {
        return (utils::assign(object{
            , 
            object::pair{std:("statistics"), object{
                object::pair{std:("conversionRate"), result["conversions"] / result["assignments"]}, 
                object::pair{std:("confidenceInterval"), this::calculateConfidenceInterval(result["conversions"], result["assignments"])}, 
                object::pair{std:("pValue"), this::calculatePValue(results, result)}
            }}
        }, result));
    }
    );
}

any redis = std::make_shared<Redis>(object{
    object::pair{std:("url"), process->env->REDIS_URL}
});

void Main(void)
{
}

MAIN
