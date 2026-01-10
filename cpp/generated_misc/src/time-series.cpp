#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/ml/time-series.h"

TimeSeriesAnalyzer::TimeSeriesAnalyzer() {
    this->prophet = std::make_shared<Prophet>(object{
        object::pair{std::string("growth"), std::string("linear")}, 
        object::pair{std::string("changepoints"), nullptr}, 
        object::pair{std::string("n_changepoints"), 25}, 
        object::pair{std::string("yearly_seasonality"), std::string("auto")}, 
        object::pair{std::string("weekly_seasonality"), std::string("auto")}, 
        object::pair{std::string("daily_seasonality"), std::string("auto")}
    });
}

std::shared_ptr<Promise<object>> TimeSeriesAnalyzer::forecast(array<object> historicalData, double horizon)
{
    try
    {
        auto df = historicalData->map([=](auto d) mutable
        {
            return (object{
                object::pair{std::string("ds"), d["timestamp"]}, 
                object::pair{std::string("y"), d["value"]}
            });
        }
        );
        std::async([=]() { this->prophet->fit(df); });
        auto forecast = std::async([=]() { this->prophet->predict(horizon); });
        auto lstmPredictions = std::async([=]() { this->getLSTMPredictions(historicalData, horizon); });
        auto arimaPredictions = std::async([=]() { this->getARIMAPredictions(historicalData, horizon); });
        auto ensemblePredictions = this->ensemblePredictions(forecast->yhat, lstmPredictions, arimaPredictions);
        return object{
            object::pair{std::string("predictions"), ensemblePredictions}, 
            object::pair{std::string("confidence"), object{
                object::pair{std::string("upper"), forecast->yhat_upper}, 
                object::pair{std::string("lower"), forecast->yhat_lower}
            }}, 
            object::pair{std::string("seasonality"), std::async([=]() { this->prophet->get_seasonality(); })}, 
            object::pair{std::string("changepoints"), forecast->changepoints}
        };
    }
    catch (const any& error)
    {
        logger["error"](std::string("Error in time series forecasting:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<array<double>>> TimeSeriesAnalyzer::getLSTMPredictions(array<object> historicalData, double horizon)
{
    if (!this->lstmModel) {
        std::async([=]() { this->trainLSTM(historicalData); });
    }
    auto sequence = this->prepareSequence(historicalData);
    auto predictions = array<any>();
    for (auto i = 0; i < horizon; i++)
    {
        auto prediction = as<std::shared_ptr<tf::Tensor>>(this->lstmModel->predict(sequence));
        predictions->push(std::async([=]() { prediction->data(); }));
        sequence->shift();
        sequence->push(const_(predictions)[predictions->get_length() - 1]);
    }
    return predictions;
}

void TimeSeriesAnalyzer::trainLSTM(array<object> historicalData)
{
    auto sequences = this->prepareTrainingData(historicalData);
    this->lstmModel = tf->sequential(object{
        object::pair{std::string("layers"), array<any>{ tf->layers->lstm(object{
            object::pair{std::string("units"), 50}, 
            object::pair{std::string("returnSequences"), true}, 
            object::pair{std::string("inputShape"), array<double>{ 30, 1 }}
        }), tf->layers->dropout(object{
            object::pair{std::string("rate"), 0.2}
        }), tf->layers->lstm(object{
            object::pair{std::string("units"), 50}
        }), tf->layers->dense(object{
            object::pair{std::string("units"), 1}
        }) }}
    });
    this->lstmModel->compile(object{
        object::pair{std::string("optimizer"), tf->train->adam(0.001)}, 
        object::pair{std::string("loss"), std::string("meanSquaredError")}
    });
    std::async([=]() { this->lstmModel->fit(sequences->inputs, sequences->outputs, object{
        object::pair{std::string("epochs"), 100}, 
        object::pair{std::string("validationSplit"), 0.2}, 
        object::pair{std::string("callbacks"), object{
            object::pair{std::string("onEpochEnd"), [=](auto epoch, auto logs) mutable
            {
                logger["info"](std::string("LSTM Epoch ") + epoch + std::string(": loss = ") + logs["loss"] + string_empty);
            }
            }
        }}
    }); });
}

std::shared_ptr<Promise<array<double>>> TimeSeriesAnalyzer::getARIMAPredictions(array<object> historicalData, double horizon)
{
    auto values = historicalData->map([=](auto d) mutable
    {
        return d["value"];
    }
    );
    auto model = std::make_shared<arima>(object{
        object::pair{std::string("p"), 2}, 
        object::pair{std::string("d"), 1}, 
        object::pair{std::string("q"), 2}, 
        object::pair{std::string("verbose"), false}
    });
    std::async([=]() { model->fit(values); });
    return model->predict(horizon);
}

array<double> TimeSeriesAnalyzer::ensemblePredictions(array<double> prophetPreds, array<double> lstmPreds, array<double> arimaPreds)
{
    shared weights = array<double>{ 0.4, 0.4, 0.2 };
    return prophetPreds->map([=](auto _, auto i) mutable
    {
        return const_(prophetPreds)[i] * const_(weights)[0] + const_(lstmPreds)[i] * const_(weights)[1] + const_(arimaPreds)[i] * const_(weights)[2];
    }
    );
}

