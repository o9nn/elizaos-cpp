#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/ml/neural-architecture-search.h"

NeuralArchitectureSearch::NeuralArchitectureSearch(std::shared_ptr<NASConfig> config) {
    this->searchSpace = this->defineSearchSpace(config);
    this->maxTrials = OR((config->maxTrials), (100));
    this->optimizer = std::make_shared<EvolutionaryOptimizer>(this->searchSpace);
}

std::shared_ptr<Promise<std::shared_ptr<NASResult>>> NeuralArchitectureSearch::search(object data, object validationData)
{
    try
    {
        auto population = std::async([=]() { this->initializePopulation(); });
        auto bestArchitecture = nullptr;
        auto bestPerformance = -Infinity;
        for (auto trial = 0; trial < this->maxTrials; trial++)
        {
            auto architectures = this->optimizer->evolvePopulation(population);
            auto results = std::async([=]() { Promise->all(architectures->map([=](auto arch) mutable
            {
                return this->evaluateArchitecture(arch, data, validationData);
            }
            )); });
            this->optimizer->updatePopulation(results);
            auto currentBest = this->findBestArchitecture(results);
            if (currentBest->performance > bestPerformance) {
                bestArchitecture = currentBest->architecture;
                bestPerformance = currentBest->performance;
            }
            std::async([=]() { this->logSearchProgress(trial, results); });
        }
        return object{
            object::pair{std:("bestArchitecture"), std:("bestArchitecture")}, 
            object::pair{std:("performance"), bestPerformance}, 
            object::pair{std:("searchHistory"), this->optimizer->getSearchHistory()}, 
            object::pair{std:("analysis"), std::async([=]() { this->analyzeSearchResults(); })}
        };
    }
    catch (const any& error)
    {
        logger["error"](std:("Error in neural architecture search:"), error);
        throw any(error);
    }
}

std::shared_ptr<SearchSpace> NeuralArchitectureSearch::defineSearchSpace(std::shared_ptr<NASConfig> config)
{
    return object{
        object::pair{std:("numLayers"), object{
            object::pair{std:("min"), 1}, 
            object::pair{std:("max"), 10}
        }}, 
        object::pair{std:("layerSizes"), object{
            object::pair{std:("min"), 32}, 
            object::pair{std:("max"), 512}, 
            object::pair{std:("step"), 32}
        }}, 
        object::pair{std:("activations"), array<string>{ std:("relu"), std:("elu"), std:("selu"), std:("tanh") }}, 
        object::pair{std:("dropoutRates"), object{
            object::pair{std:("min"), 0}, 
            object::pair{std:("max"), 0.5}, 
            object::pair{std:("step"), 0.1}
        }}, 
        object::pair{std:("optimizers"), array<string>{ std:("adam"), std:("rmsprop"), std:("sgd") }}, 
        object::pair{std:("learningRates"), object{
            object::pair{std:("min"), 0.0001}, 
            object::pair{std:("max"), 0.1}, 
            object::pair{std:("log"), true}
        }}, 
        object::pair{std:("batchNorm"), array<boolean>{ true, false }}, 
        object::pair{std:("skipConnections"), array<boolean>{ true, false }}, 
        object::pair{std:("attentionLayers"), array<boolean>{ true, false }}
    };
}

std::shared_ptr<Promise<std::shared_ptr<ArchitectureEvaluation>>> NeuralArchitectureSearch::evaluateArchitecture(std::shared_ptr<NeuralArchitecture> architecture, object data, object validationData)
{
    auto model = this->buildModel(architecture);
    auto history = std::async([=]() { model->fit(data["x"], data["y"], object{
        object::pair{std:("epochs"), 50}, 
        object::pair{std:("validationData"), array<any>{ validationData["x"], validationData["y"] }}, 
        object::pair{std:("callbacks"), this->createTrainingCallbacks()}
    }); });
    auto evaluation = std::async([=]() { model->evaluate(validationData["x"], validationData["y"]); });
    return object{
        object::pair{std:("architecture"), std:("architecture")}, 
        object::pair{std:("performance"), this->calculatePerformanceMetric(evaluation, history)}, 
        object::pair{std:("complexity"), this->calculateArchitectureComplexity(architecture)}, 
        object::pair{std:("trainingTime"), history->totalTime}, 
        object::pair{std:("memory"), std::async([=]() { this->estimateMemoryUsage(model); })}
    };
}

array<std::shared_ptr<tf::Callback>> NeuralArchitectureSearch::createTrainingCallbacks()
{
    return array<any>{ tf->callbacks->earlyStopping(object{
        object::pair{std:("monitor"), std:("val_loss")}, 
        object::pair{std:("patience"), 5}
    }), tf->callbacks->learningRateScheduler([=](auto epoch, auto lr) mutable
    {
        return lr * (Math.pow(0.95, epoch));
    }
    ) };
}

