#include "time-series-decomposition.h"

TimeSeriesDecomposer::TimeSeriesDecomposer() {
    this->stl = std::make_shared<STL>(object{
        object::pair{std:("periodicity"), 24 * 7}, 
        object::pair{std:("robustness"), true}, 
        object::pair{std:("numberOfInnerLoops"), 2}, 
        object::pair{std:("numberOfOuterLoops"), 1}, 
        object::pair{std:("seasonalDegree"), 1}, 
        object::pair{std:("trendDegree"), 1}, 
        object::pair{std:("lowpassDegree"), 1}
    });
    this->wavelet = std::make_shared<WaveletTransform>(std:("db4"));
}

std::shared_ptr<Promise<std::shared_ptr<DecompositionResult>>> TimeSeriesDecomposer::decompose(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config)
{
    try
    {
        auto [stlResult, waveletResult, empiricalResult] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ this->performSTLDecomposition(timeSeries, config), this->performWaveletDecomposition(timeSeries, config), this->performEMD(timeSeries) }); });
        auto components = this->combineDecompositions(stlResult, waveletResult, empiricalResult);
        return object{
            object::pair{std:("components"), std:("components")}, 
            object::pair{std:("analysis"), std::async([=]() { this->analyzeComponents(components); })}, 
            object::pair{std:("seasonality"), this->detectSeasonality(components)}, 
            object::pair{std:("changepoints"), std::async([=]() { this->detectChangepoints(timeSeries, components); })}, 
            object::pair{std:("forecast"), std::async([=]() { this->generateComponentForecasts(components, config); })}
        };
    }
    catch (const any& error)
    {
        logger["error"](std:("Error in time series decomposition:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<std::shared_ptr<STLResult>>> TimeSeriesDecomposer::performSTLDecomposition(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config)
{
    auto result = std::async([=]() { this->stl->fit(timeSeries); });
    return object{
        object::pair{std:("trend"), result->trend}, 
        object::pair{std:("seasonal"), result->seasonal}, 
        object::pair{std:("residual"), result->residual}, 
        object::pair{std:("robustness"), result->weights}, 
        object::pair{std:("diagnostics"), this->calculateSTLDiagnostics(result)}
    };
}

std::shared_ptr<Promise<std::shared_ptr<WaveletResult>>> TimeSeriesDecomposer::performWaveletDecomposition(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config)
{
    auto levels = Math->min(Math->floor(Math->log2(timeSeries->get_length())), OR((config->maxWaveletLevel), (5)));
    auto coefficients = this->wavelet->transform(timeSeries, levels);
    auto components = this->reconstructWaveletComponents(coefficients, levels);
    return object{
        object::pair{std:("components"), std:("components")}, 
        object::pair{std:("coefficients"), std:("coefficients")}, 
        object::pair{std:("energyDistribution"), this->calculateWaveletEnergy(coefficients)}, 
        object::pair{std:("significance"), std::async([=]() { this->testWaveletSignificance(components); })}
    };
}

std::shared_ptr<Promise<std::shared_ptr<EMDResult>>> TimeSeriesDecomposer::performEMD(array<double> timeSeries)
{
    auto imfs = std::async([=]() { this->empiricalModeDecomposition(timeSeries); });
    return object{
        object::pair{std:("imfs"), std:("imfs")}, 
        object::pair{std:("instantaneousFrequency"), this->calculateInstantaneousFrequency(imfs)}, 
        object::pair{std:("hilbertSpectrum"), std::async([=]() { this->computeHilbertSpectrum(imfs); })}
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
        object::pair{std:("trend"), std::async([=]() { this->forecastTrend(components->trend, horizon); })}, 
        object::pair{std:("seasonal"), std::async([=]() { this->forecastSeasonal(components->seasonal, horizon); })}, 
        object::pair{std:("combined"), std::async([=]() { this->generateCombinedForecast(components, horizon); })}
    };
}

