#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics/visualization.h"

std::shared_ptr<ChartConfiguration> AnalyticsVisualization::generateTimeSeriesChart(array<std::shared_ptr<TimeSeriesDataPoint>> data)
{
    return object{
        object::pair{std::string("type"), std::string("line")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("labels"), data->map([=](auto d) mutable
            {
                return d->date;
            }
            )}, 
            object::pair{std::string("datasets"), array<object>{ object{
                object::pair{std::string("label"), std::string("Views")}, 
                object::pair{std::string("data"), data->map([=](auto d) mutable
                {
                    return d->value;
                }
                )}
            } }}
        }}, 
        object::pair{std::string("options"), object{
            object::pair{std::string("responsive"), true}
        }}
    };
}

std::shared_ptr<ChartConfiguration> AnalyticsVisualization::generateHeatmap(std::shared_ptr<InteractionData> data)
{
    return object{
        object::pair{std::string("type"), std::string("bar")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("labels"), Object->keys(data)}, 
            object::pair{std::string("datasets"), array<object>{ object{
                object::pair{std::string("label"), std::string("Interactions")}, 
                object::pair{std::string("data"), Object->values(data)}
            } }}
        }}, 
        object::pair{std::string("options"), object{
            object::pair{std::string("responsive"), true}
        }}
    };
}

