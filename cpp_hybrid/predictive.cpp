#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics/predictive.h"

PredictiveAnalytics::PredictiveAnalytics() {
    this->mlPredictor = std::make_shared<MLPredictor>();
    this->initialize();
}

void PredictiveAnalytics::initialize()
{
    std::async([=]() { this->mlPredictor->loadModel(); });
}

std::shared_ptr<Promise<object>> PredictiveAnalytics::predictProjectSuccess(string projectId)
{
    try
    {
        auto features = std::async([=]() { this->extractProjectFeatures(projectId); });
        auto score = std::async([=]() { this->mlPredictor->predictEngagement(features); });
        return object{
            object::pair{std::string("score"), std::string("score")}, 
            object::pair{std::string("factors"), this->analyzeContributingFactors(features)}, 
            object::pair{std::string("recommendations"), this->generateRecommendations(features, score)}
        };
    }
    catch (const any& error)
    {
        logger["error"](std::string("Error predicting project success:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<object>> PredictiveAnalytics::predictUserEngagement(string userId)
{
    auto userFeatures = std::async([=]() { this->extractUserFeatures(userId); });
    auto likelihood = std::async([=]() { this->mlPredictor->predictEngagement(userFeatures); });
    return object{
        object::pair{std::string("likelihood"), std::string("likelihood")}, 
        object::pair{std::string("nextActions"), this->suggestNextActions(userFeatures)}, 
        object::pair{std::string("interests"), std::async([=]() { this->predictUserInterests(userFeatures); })}
    };
}

