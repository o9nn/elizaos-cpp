#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/ml/anomaly-detection.h"

AnomalyDetector::AnomalyDetector() {
    this->isolationForest = std::make_shared<IsolationForest>(object{
        object::pair{std::string("numberOfTrees"), 100}, 
        object::pair{std::string("maxSamples"), 256}, 
        object::pair{std::string("contamination"), 0.1}
    });
    this->dbscan = std::make_shared<DBSCAN>();
}

void AnomalyDetector::initialize()
{
    std::async([=]() { this->buildAutoencoder(); });
}

void AnomalyDetector::buildAutoencoder()
{
    auto encoder = tf->sequential(object{
        object::pair{std::string("layers"), array<any>{ tf->layers->dense(object{
            object::pair{std::string("inputShape"), array<double>{ 50 }}, 
            object::pair{std::string("units"), 32}, 
            object::pair{std::string("activation"), std::string("relu")}
        }), tf->layers->dense(object{
            object::pair{std::string("units"), 16}, 
            object::pair{std::string("activation"), std::string("relu")}
        }), tf->layers->dense(object{
            object::pair{std::string("units"), 8}, 
            object::pair{std::string("activation"), std::string("relu")}
        }), tf->layers->dense(object{
            object::pair{std::string("units"), 4}, 
            object::pair{std::string("activation"), std::string("relu")}
        }) }}
    });
    auto decoder = tf->sequential(object{
        object::pair{std::string("layers"), array<any>{ tf->layers->dense(object{
            object::pair{std::string("inputShape"), array<double>{ 4 }}, 
            object::pair{std::string("units"), 8}, 
            object::pair{std::string("activation"), std::string("relu")}
        }), tf->layers->dense(object{
            object::pair{std::string("units"), 16}, 
            object::pair{std::string("activation"), std::string("relu")}
        }), tf->layers->dense(object{
            object::pair{std::string("units"), 32}, 
            object::pair{std::string("activation"), std::string("relu")}
        }), tf->layers->dense(object{
            object::pair{std::string("units"), 50}, 
            object::pair{std::string("activation"), std::string("sigmoid")}
        }) }}
    });
    this->autoencoder = tf->sequential(object{
        object::pair{std::string("layers"), array<any>{ encoder->layers, decoder->layers }}
    });
    this->autoencoder->compile(object{
        object::pair{std::string("optimizer"), std::string("adam")}, 
        object::pair{std::string("loss"), std::string("meanSquaredError")}
    });
}

std::shared_ptr<Promise<array<std::shared_ptr<AnomalyResult>>>> AnomalyDetector::detectAnomalies(array<array<double>> data)
{
    auto results = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<array<double>>>, std::shared_ptr<Promise<array<double>>>, array<double>>{ this->detectWithAutoencoder(data), this->detectWithIsolationForest(data), this->detectWithDBSCAN(data) }); });
    return this->ensembleResults(results);
}

std::shared_ptr<Promise<array<double>>> AnomalyDetector::detectWithAutoencoder(array<array<double>> data)
{
    auto tensor = tf->tensor2d(data);
    auto reconstructed = as<std::shared_ptr<tf::Tensor>>(this->autoencoder->predict(tensor));
    auto losses = tf->losses->meanSquaredError(tensor, reconstructed);
    return std::async([=]() { losses->array(); });
}

std::shared_ptr<Promise<array<double>>> AnomalyDetector::detectWithIsolationForest(array<array<double>> data)
{
    this->isolationForest->fit(data);
    return data->map([=](auto point) mutable
    {
        return const_(this->isolationForest->predict(array<array<double>>{ point }))[0];
    }
    );
}

array<double> AnomalyDetector::detectWithDBSCAN(array<array<double>> data)
{
    auto clusters = this->dbscan->run(data, 0.5, 3);
    shared outliers = std::make_shared<Set>(this->dbscan->noise);
    return data->map([=](auto _, auto index) mutable
    {
        return (outliers->has(index)) ? 1 : 0;
    }
    );
}

array<std::shared_ptr<AnomalyResult>> AnomalyDetector::ensembleResults(array<array<double>> results)
{
    return const_(results)[0]->map([=](auto _, auto i) mutable
    {
        auto score = (const_(const_(results)[0])[i] + const_(const_(results)[1])[i] + const_(const_(results)[2])[i]) / 3;
        return object{
            object::pair{std::string("index"), i}, 
            object::pair{std::string("score"), std::string("score")}, 
            object::pair{std::string("isAnomaly"), score > this->threshold}, 
            object::pair{std::string("confidence"), this->calculateConfidence(results->map([=](auto r) mutable
            {
                return const_(r)[i];
            }
            ))}
        };
    }
    );
}

