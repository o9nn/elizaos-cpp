#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/ml/time-series-decomposition.h"

TimeSeriesDecomposer::TimeSeriesDecomposer() {
    this->stl = std::make_shared<STL>(object{
        object::pair{std::string("periodicity"), 24 * 7}, 
        object::pair{std::string("robustness"), true}, 
        object::pair{std::string("numberOfInnerLoops"), 2}, 
        object::pair{std::string("numberOfOuterLoops"), 1}, 
        object::pair{std::string("seasonalDegree"), 1}, 
        object::pair{std::string("trendDegree"), 1}, 
        object::pair{std::string("lowpassDegree"), 1}
    });
    this->wavelet = std::make_shared<WaveletTransform>(std::string("db4"));
}

std::shared_ptr<Promise<std::shared_ptr<DecompositionResult>>> TimeSeriesDecomposer::decompose(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config)
{
    try
    {
        auto [stlResult, waveletResult, empiricalResult] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ this->performSTLDecomposition(timeSeries, config), this->performWaveletDecomposition(timeSeries, config), this->performEMD(timeSeries) }); });
        auto components = this->combineDecompositions(stlResult, waveletResult, empiricalResult);
        return object{
            object::pair{std::string("components"), std::string("components")}, 
            object::pair{std::string("analysis"), std::async([=]() { this->analyzeComponents(components); })}, 
            object::pair{std::string("seasonality"), this->detectSeasonality(components)}, 
            object::pair{std::string("changepoints"), std::async([=]() { this->detectChangepoints(timeSeries, components); })}, 
            object::pair{std::string("forecast"), std::async([=]() { this->generateComponentForecasts(components, config); })}
        };
    }
    catch (const any& error)
    {
        logger["error"](std::string("Error in time series decomposition:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<std::shared_ptr<STLResult>>> TimeSeriesDecomposer::performSTLDecomposition(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config)
{
    auto result = std::async([=]() { this->stl->fit(timeSeries); });
    return object{
        object::pair{std::string("trend"), result->trend}, 
        object::pair{std::string("seasonal"), result->seasonal}, 
        object::pair{std::string("residual"), result->residual}, 
        object::pair{std::string("robustness"), result->weights}, 
        object::pair{std::string("diagnostics"), this->calculateSTLDiagnostics(result)}
    };
}

std::shared_ptr<Promise<std::shared_ptr<WaveletResult>>> TimeSeriesDecomposer::performWaveletDecomposition(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config)
{
    auto levels = Math->min(Math->floor(Math->log2(timeSeries->get_length())), OR((config->maxWaveletLevel), (5)));
    auto coefficients = this->wavelet->transform(timeSeries, levels);
    auto components = this->reconstructWaveletComponents(coefficients, levels);
    return object{
        object::pair{std::string("components"), std::string("components")}, 
        object::pair{std::string("coefficients"), std::string("coefficients")}, 
        object::pair{std::string("energyDistribution"), this->calculateWaveletEnergy(coefficients)}, 
        object::pair{std::string("significance"), std::async([=]() { this->testWaveletSignificance(components); })}
    };
}

std::shared_ptr<Promise<std::shared_ptr<EMDResult>>> TimeSeriesDecomposer::performEMD(array<double> timeSeries)
{
    auto imfs = std::async([=]() { this->empiricalModeDecomposition(timeSeries); });
    return object{
        object::pair{std::string("imfs"), std::string("imfs")}, 
        object::pair{std::string("instantaneousFrequency"), this->calculateInstantaneousFrequency(imfs)}, 
        object::pair{std::string("hilbertSpectrum"), std::async([=]() { this->computeHilbertSpectrum(imfs); })}
    };
}

std::shared_ptr<Promise<array<std::shared_ptr<Changepoint>>>> TimeSeriesDecomposer::detectChangepoints(array<double> timeSeries, std::shared_ptr<DecompositionComponents> components)
{
    auto [trendChangepoints, seasonalChangepoints, structuralBreaks] = std::async([=]() { Promise->all(std::tuple<any, any, any>{ this->detectTrendChangepoints(components->trend), this->detectSeasonalChangepoints(components->seasonal), this->detectStructuralBreaks(timeSeries) }); });
    return this->mergeChangepoints(trendChangepoints, seasonalChangepoints, structuralBreaks);
}

std::shared_ptr<Promise<std::shared_ptr<ComponentForecasts>>> TimeSeriesDecomposer::generateComponentForecasts(std::shared_ptr<DecompositionComponents> components, std::shared_ptr<DecompositionConfig> config)
{
    auto horizon = OR((config->forecastHorizon), (30));
    return object{
        object::pair{std::string("trend"), std::async([=]() { this->forecastTrend(components->trend, horizon); })}, 
        object::pair{std::string("seasonal"), std::async([=]() { this->forecastSeasonal(components->seasonal, horizon); })}, 
        object::pair{std::string("combined"), std::async([=]() { this->generateCombinedForecast(components, horizon); })}
    };
}

