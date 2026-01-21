#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analysis/feature-engineering.h"

std::shared_ptr<Promise<std::shared_ptr<EngineeredFeatures>>> FeatureEngineering::engineerFeatures(array<Record<string, any>> data, std::shared_ptr<FeatureConfig> config)
{
    try
    {
        auto [numericalFeatures, categoricalFeatures, temporalFeatures, interactionFeatures] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<array<any>>>, std::shared_ptr<Promise<array<any>>>, any, any>{ this->processNumericalFeatures(data, config->numerical), this->processCategoricalFeatures(data, config->categorical), this->processTemporalFeatures(data, config->temporal), this->createInteractionFeatures(data, config->interactions) }); });
        auto reducedFeatures = std::async([=]() { this->reduceDimensionality(array<any>{ numericalFeatures, categoricalFeatures }, config->dimensionality); });
        auto selectedFeatures = std::async([=]() { this->selectFeatures(reducedFeatures, config->target, config->selectionCriteria); });
        return object{
            object::pair{std::string("features"), selectedFeatures}, 
            object::pair{std::string("metadata"), this->generateFeatureMetadata(selectedFeatures)}, 
            object::pair{std::string("importance"), std::async([=]() { this->calculateFeatureImportance(selectedFeatures, config->target); })}
        };
    }
    catch (const any& error)
    {
        logger["error"](std::string("Error in feature engineering:"), error);
        throw any(error);
    }
}

any FeatureEngineering::processNumericalFeatures(array<Record<string, any>> data, std::shared_ptr<NumericalFeatureConfig> config)
{
    auto features = array<any>();
    for (auto& feature : config->features)
    {
        features->push(this->normalize(data, feature), this->standardize(data, feature), this->polynomialFeatures(data, feature, config->degree));
        if (config->advanced) {
            features->push(this->boxCoxTransform(data, feature), this->yeojohnsonTransform(data, feature), this->robustScaling(data, feature));
        }
    }
    return features;
}

any FeatureEngineering::processCategoricalFeatures(array<Record<string, any>> data, std::shared_ptr<CategoricalFeatureConfig> config)
{
    auto features = array<any>();
    for (auto& feature : config->features)
    {
        static switch_type __switch2308_2867 = {
            { any(std::string("onehot")), 1 },
            { any(std::string("target")), 2 },
            { any(std::string("frequency")), 3 },
            { any(std::string("embedding")), 4 }
        };
        switch (__switch2308_2867[config->encoding])
        {
        case 1:
            features->push(std::async([=]() { this->oneHotEncode(data, feature); }));
            break;
        case 2:
            features->push(std::async([=]() { this->targetEncode(data, feature, config->target); }));
            break;
        case 3:
            features->push(std::async([=]() { this->frequencyEncode(data, feature); }));
            break;
        case 4:
            features->push(std::async([=]() { this->categoryEmbedding(data, feature, config->embeddingDim); }));
            break;
        }
    }
    return features;
}

