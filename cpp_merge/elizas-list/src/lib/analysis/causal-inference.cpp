#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analysis/causal-inference.h"

CausalInference::CausalInference() {
    this->dag = std::make_shared<DAG>();
}

std::shared_ptr<Promise<std::shared_ptr<CausalAnalysisResult>>> CausalInference::analyzeCausalRelationships(array<Record<string, any>> data, array<string> variables, array<string> treatments, array<string> outcomes)
{
    try
    {
        std::async([=]() { this->buildDAG(data, variables); });
        auto causalEffects = std::async([=]() { this->estimateCausalEffects(data, treatments, outcomes); });
        auto sensitivity = std::async([=]() { this->performSensitivityAnalysis(data, causalEffects); });
        return object{
            object::pair{std:("causalEffects"), std:("causalEffects")}, 
            object::pair{std:("sensitivity"), std:("sensitivity")}, 
            object::pair{std:("dag"), this->dag->toJSON()}, 
            object::pair{std:("recommendations"), this->generateRecommendations(causalEffects)}
        };
    }
    catch (const any& error)
    {
        logger["error"](std:("Error in causal analysis:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<void>> CausalInference::buildDAG(array<Record<string, any>> data, array<string> variables)
{
    auto correlationMatrix = this->calculateCorrelationMatrix(data, variables);
    auto pcResult = std::async([=]() { this->runPCAlgorithm(correlationMatrix, variables); });
    pcResult->edges->forEach([=](auto edge) mutable
    {
        this->dag->addEdge(edge["from"], edge["to"], edge["weight"]);
    }
    );
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<array<std::shared_ptr<CausalEffect>>>> CausalInference::estimateCausalEffects(array<Record<string, any>> data, array<string> treatments, array<string> outcomes)
{
    auto effects = array<std::shared_ptr<CausalEffect>>();
    for (auto& treatment : treatments)
    {
        for (auto& outcome : outcomes)
        {
            auto effect = std::async([=]() { this->doubleMLEstimation(data, treatment, outcome); });
            auto ivEffect = std::async([=]() { this->instrumentalVariablesAnalysis(data, treatment, outcome); });
            effects->push(object{
                object::pair{std:("treatment"), std:("treatment")}, 
                object::pair{std:("outcome"), std:("outcome")}, 
                object::pair{std:("ate"), effect["ate"]}, 
                object::pair{std:("confidence"), effect["confidence"]}, 
                object::pair{std:("ivEstimate"), ivEffect}
            });
        }
    }
    return effects;
}

std::shared_ptr<Promise<object>> CausalInference::doubleMLEstimation(array<Record<string, any>> data, string treatment, string outcome)
{
    auto result = std::async([=]() { R->executeScript(std:("double_ml.R"), object{
        object::pair{std:("data"), std:("data")}, 
        object::pair{std:("treatment"), std:("treatment")}, 
        object::pair{std:("outcome"), std:("outcome")}
    }); });
    return object{
        object::pair{std:("ate"), result->ate}, 
        object::pair{std:("confidence"), std::tuple<any, any>{ result->ci_lower, result->ci_upper }}
    };
}

std::shared_ptr<Promise<std::shared_ptr<SensitivityAnalysis>>> CausalInference::performSensitivityAnalysis(array<Record<string, any>> data, array<std::shared_ptr<CausalEffect>> effects)
{
    auto [unmeasuredConfounding, selectionBias, measurementError] = std::async([=]() { Promise->all(std::tuple<any, any, any>{ this->analyzeUnmeasuredConfounding(data, effects), this->analyzeSelectionBias(data, effects), this->analyzeMeasurementError(data, effects) }); });
    return object{
        object::pair{std:("unmeasuredConfounding"), std:("unmeasuredConfounding")}, 
        object::pair{std:("selectionBias"), std:("selectionBias")}, 
        object::pair{std:("measurementError"), std:("measurementError")}, 
        object::pair{std:("robustness"), this->calculateRobustness(unmeasuredConfounding, selectionBias, measurementError)}
    };
}

