#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/ml/time-series.h"

TimeSeriesAnalyzer::TimeSeriesAnalyzer() {
    this->prophet = std::make_shared<Prophet>(object{
        object::pair{std:("growth"), std:("linear")}, 
        object::pair{std:("changepoints"), nullptr}, 
        object::pair{std:("n_changepoints"), 25}, 
        object::pair{std:("yearly_seasonality"), std:("auto")}, 
        object::pair{std:("weekly_seasonality"), std:("auto")}, 
        object::pair{std:("daily_seasonality"), std:("auto")}
    });
}

std::shared_ptr<Promise<object>> TimeSeriesAnalyzer::forecast(array<object> historicalData, double horizon)
{
    try
    {
        auto df = historicalData->map([=](auto d) mutable
        {
            return (object{
                object::pair{std:("ds"), d["timestamp"]}, 
                object::pair{std:("y"), d["value"]}
            });
        }
        );
        std::async([=]() { this->prophet->fit(df); });
        auto forecast = std::async([=]() { this->prophet->predict(horizon); });
        auto lstmPredictions = std::async([=]() { this->getLSTMPredictions(historicalData, horizon); });
        auto arimaPredictions = std::async([=]() { this->getARIMAPredictions(historicalData, horizon); });
        auto ensemblePredictions = this->ensemblePredictions(forecast->yhat, lstmPredictions, arimaPredictions);
        return object{
            object::pair{std:("predictions"), ensemblePredictions}, 
            object::pair{std:("confidence"), object{
                object::pair{std:("upper"), forecast->yhat_upper}, 
                object::pair{std:("lower"), forecast->yhat_lower}
            }}, 
            object::pair{std:("seasonality"), std::async([=]() { this->prophet->get_seasonality(); })}, 
            object::pair{std:("changepoints"), forecast->changepoints}
        };
    }
    catch (const any& error)
    {
        logger["error"](std:("Error in time series forecasting:"), error);
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
        object::pair{std:("layers"), array<any>{ tf->layers->lstm(object{
            object::pair{std:("units"), 50}, 
            object::pair{std:("returnSequences"), true}, 
            object::pair{std:("inputShape"), array<double>{ 30, 1 }}
        }), tf->layers->dropout(object{
            object::pair{std:("rate"), 0.2}
        }), tf->layers->lstm(object{
            object::pair{std:("units"), 50}
        }), tf->layers->dense(object{
            object::pair{std:("units"), 1}
        }) }}
    });
    this->lstmModel->compile(object{
        object::pair{std:("optimizer"), tf->train->adam(0.001)}, 
        object::pair{std:("loss"), std:("meanSquaredError")}
    });
    std::async([=]() { this->lstmModel->fit(sequences->inputs, sequences->outputs, object{
        object::pair{std:("epochs"), 100}, 
        object::pair{std:("validationSplit"), 0.2}, 
        object::pair{std:("callbacks"), object{
            object::pair{std:("onEpochEnd"), [=](auto epoch, auto logs) mutable
            {
                logger["info"](std:("LSTM Epoch ") + epoch + std:(": loss = ") + logs["loss"] + string_empty);
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
        object::pair{std:("p"), 2}, 
        object::pair{std:("d"), 1}, 
        object::pair{std:("q"), 2}, 
        object::pair{std:("verbose"), false}
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

