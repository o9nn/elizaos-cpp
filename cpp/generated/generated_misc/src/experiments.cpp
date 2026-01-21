#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/ab-testing/experiments.h"

std::shared_ptr<Promise<string>> ABTestingService::assignVariant(string experimentId, string userId)
{
    auto experiment = std::async([=]() { ABTestingService::getExperiment(experimentId); });
    if (!experiment) throw any(std::make_shared<Error>(std::string("Experiment not found")));
    auto existingVariant = std::async([=]() { redis->get(std::string("ab:") + experimentId + std::string(":") + userId + string_empty); });
    if (existingVariant) return existingVariant;
    auto variant = ABTestingService::selectVariant(experiment->variants);
    std::async([=]() { redis->set(std::string("ab:") + experimentId + std::string(":") + userId + string_empty, variant->id); });
    std::async([=]() { ABTestingService::trackAssignment(experimentId, userId, variant->id); });
    return variant->id;
}

void ABTestingService::trackConversion(string experimentId, string userId, string conversionType, double value)
{
    auto variantId = std::async([=]() { redis->get(std::string("ab:") + experimentId + std::string(":") + userId + string_empty); });
    if (!variantId) return std::shared_ptr<Promise<void>>();
    std::async([=]() { Promise->all(std::tuple<any, any>{ redis->hincrby(std::string("ab:conversions:") + experimentId + std::string(":") + variantId + string_empty, conversionType, 1), AND((value), (redis->hincrbyfloat(std::string("ab:values:") + experimentId + std::string(":") + variantId + string_empty, conversionType, value))) }); });
}

any ABTestingService::getResults(string experimentId)
{
    auto experiment = std::async([=]() { ABTestingService::getExperiment(experimentId); });
    if (!experiment) throw any(std::make_shared<Error>(std::string("Experiment not found")));
    auto results = std::async([=]() { Promise->all(experiment->variants->map([=](auto variant) mutable
    {
        auto [assignments, conversions, values] = std::async([=]() { Promise->all(std::tuple<any, any, any>{ redis->get(std::string("ab:assignments:") + experimentId + std::string(":") + variant["id"] + string_empty), redis->hgetall(std::string("ab:conversions:") + experimentId + std::string(":") + variant["id"] + string_empty), redis->hgetall(std::string("ab:values:") + experimentId + std::string(":") + variant["id"] + string_empty) }); });
        return object{
            object::pair{std::string("variantId"), variant["id"]}, 
            object::pair{std::string("assignments"), parseInt(OR((assignments), (std::string("0"))))}, 
            object::pair{std::string("conversions"), std::string("conversions")}, 
            object::pair{std::string("values"), std::string("values")}
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
            object::pair{std::string("statistics"), object{
                object::pair{std::string("conversionRate"), result["conversions"] / result["assignments"]}, 
                object::pair{std::string("confidenceInterval"), this::calculateConfidenceInterval(result["conversions"], result["assignments"])}, 
                object::pair{std::string("pValue"), this::calculatePValue(results, result)}
            }}
        }, result));
    }
    );
}

any redis = std::make_shared<Redis>(object{
    object::pair{std::string("url"), process->env->REDIS_URL}
});

void Main(void)
{
}

MAIN
