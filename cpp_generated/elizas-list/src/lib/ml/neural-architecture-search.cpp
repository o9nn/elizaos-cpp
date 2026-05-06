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
            object::pair{std::string("bestArchitecture"), std::string("bestArchitecture")}, 
            object::pair{std::string("performance"), bestPerformance}, 
            object::pair{std::string("searchHistory"), this->optimizer->getSearchHistory()}, 
            object::pair{std::string("analysis"), std::async([=]() { this->analyzeSearchResults(); })}
        };
    }
    catch (const any& error)
    {
        logger["error"](std::string("Error in neural architecture search:"), error);
        throw any(error);
    }
}

std::shared_ptr<SearchSpace> NeuralArchitectureSearch::defineSearchSpace(std::shared_ptr<NASConfig> config)
{
    return object{
        object::pair{std::string("numLayers"), object{
            object::pair{std::string("min"), 1}, 
            object::pair{std::string("max"), 10}
        }}, 
        object::pair{std::string("layerSizes"), object{
            object::pair{std::string("min"), 32}, 
            object::pair{std::string("max"), 512}, 
            object::pair{std::string("step"), 32}
        }}, 
        object::pair{std::string("activations"), array<string>{ std::string("relu"), std::string("elu"), std::string("selu"), std::string("tanh") }}, 
        object::pair{std::string("dropoutRates"), object{
            object::pair{std::string("min"), 0}, 
            object::pair{std::string("max"), 0.5}, 
            object::pair{std::string("step"), 0.1}
        }}, 
        object::pair{std::string("optimizers"), array<string>{ std::string("adam"), std::string("rmsprop"), std::string("sgd") }}, 
        object::pair{std::string("learningRates"), object{
            object::pair{std::string("min"), 0.0001}, 
            object::pair{std::string("max"), 0.1}, 
            object::pair{std::string("log"), true}
        }}, 
        object::pair{std::string("batchNorm"), array<boolean>{ true, false }}, 
        object::pair{std::string("skipConnections"), array<boolean>{ true, false }}, 
        object::pair{std::string("attentionLayers"), array<boolean>{ true, false }}
    };
}

std::shared_ptr<Promise<std::shared_ptr<ArchitectureEvaluation>>> NeuralArchitectureSearch::evaluateArchitecture(std::shared_ptr<NeuralArchitecture> architecture, object data, object validationData)
{
    auto model = this->buildModel(architecture);
    auto history = std::async([=]() { model->fit(data["x"], data["y"], object{
        object::pair{std::string("epochs"), 50}, 
        object::pair{std::string("validationData"), array<any>{ validationData["x"], validationData["y"] }}, 
        object::pair{std::string("callbacks"), this->createTrainingCallbacks()}
    }); });
    auto evaluation = std::async([=]() { model->evaluate(validationData["x"], validationData["y"]); });
    return object{
        object::pair{std::string("architecture"), std::string("architecture")}, 
        object::pair{std::string("performance"), this->calculatePerformanceMetric(evaluation, history)}, 
        object::pair{std::string("complexity"), this->calculateArchitectureComplexity(architecture)}, 
        object::pair{std::string("trainingTime"), history->totalTime}, 
        object::pair{std::string("memory"), std::async([=]() { this->estimateMemoryUsage(model); })}
    };
}

array<std::shared_ptr<tf::Callback>> NeuralArchitectureSearch::createTrainingCallbacks()
{
    return array<any>{ tf->callbacks->earlyStopping(object{
        object::pair{std::string("monitor"), std::string("val_loss")}, 
        object::pair{std::string("patience"), 5}
    }), tf->callbacks->learningRateScheduler([=](auto epoch, auto lr) mutable
    {
        return lr * (Math.pow(0.95, epoch));
    }
    ) };
}

