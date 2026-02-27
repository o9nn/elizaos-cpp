#include "visualization.hpp"

std::shared_ptr<ChartConfiguration> AnalyticsVisualization::generateTimeSeriesChart(array<std::shared_ptr<TimeSeriesDataPoint>> data)
{
    return object{
        object::pair{std:("type"), std:("line")}, 
        object::pair{std:("data"), object{
            object::pair{std:("labels"), data->map([=](auto d) mutable
            {
                return d->date;
            }
            )}, 
            object::pair{std:("datasets"), array<object>{ object{
                object::pair{std:("label"), std:("Views")}, 
                object::pair{std:("data"), data->map([=](auto d) mutable
                {
                    return d->value;
                }
                )}
            } }}
        }}, 
        object::pair{std:("options"), object{
            object::pair{std:("responsive"), true}
        }}
    };
}

std::shared_ptr<ChartConfiguration> AnalyticsVisualization::generateHeatmap(std::shared_ptr<InteractionData> data)
{
    return object{
        object::pair{std:("type"), std:("bar")}, 
        object::pair{std:("data"), object{
            object::pair{std:("labels"), Object->keys(data)}, 
            object::pair{std:("datasets"), array<object>{ object{
                object::pair{std:("label"), std:("Interactions")}, 
                object::pair{std:("data"), Object->values(data)}
            } }}
        }}, 
        object::pair{std:("options"), object{
            object::pair{std:("responsive"), true}
        }}
    };
}

