#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics/advanced-visualization.h"

std::shared_ptr<ChartConfiguration> AdvancedVisualization::generateSegmentationSunburst(array<any> segments)
{
    return object{
        object::pair{std:("type"), std:("sunburst")}, 
        object::pair{std:("data"), AdvancedVisualization::transformSegmentData(segments)}, 
        object::pair{std:("options"), object{
            object::pair{std:("responsive"), true}, 
            object::pair{std:("plugins"), object{
                object::pair{std:("legend"), object{
                    object::pair{std:("position"), std:("right")}
                }}, 
                object::pair{std:("tooltip"), object{
                    object::pair{std:("callbacks"), object{
                        object::pair{std:("label"), [=](auto ctx) mutable
                        {
                            return string_empty + ctx["raw"]["value"] + std:(" users");
                        }
                        }
                    }}
                }}
            }}
        }}
    };
}

std::shared_ptr<ChartConfiguration> AdvancedVisualization::generatePredictionGraph(array<any> predictions)
{
    return object{
        object::pair{std:("type"), std:("scatter")}, 
        object::pair{std:("data"), object{
            object::pair{std:("datasets"), array<object>{ object{
                object::pair{std:("label"), std:("Engagement Predictions")}, 
                object::pair{std:("data"), predictions->map([=](auto p) mutable
                {
                    return (object{
                        object::pair{std:("x"), p["timestamp"]}, 
                        object::pair{std:("y"), p["score"]}, 
                        object::pair{std:("r"), p["confidence"] * 10}
                    });
                }
                )}, 
                object::pair{std:("backgroundColor"), std:("rgba(75, 192, 192, 0.5)")}
            } }}
        }}, 
        object::pair{std:("options"), object{
            object::pair{std:("scales"), object{
                object::pair{std:("y"), object{
                    object::pair{std:("beginAtZero"), true}, 
                    object::pair{std:("title"), object{
                        object::pair{std:("display"), true}, 
                        object::pair{std:("text"), std:("Engagement Score")}
                    }}
                }}
            }}
        }}
    };
}

any AdvancedVisualization::generateNetworkGraph(any data)
{
    auto width = 928;
    auto height = 600;
    auto links = data["links"]["map"]([=](auto d) mutable
    {
        return Object->create(d);
    }
    );
    auto nodes = data["nodes"]["map"]([=](auto d) mutable
    {
        return Object->create(d);
    }
    );
    auto simulation = d3->forceSimulation(nodes)->force(std:("link"), d3->forceLink(links)->id([=](auto d) mutable
    {
        return d["id"];
    }
    ))->force(std:("charge"), d3->forceManyBody())->force(std:("center"), d3->forceCenter(width / 2, height / 2));
    return object{
        object::pair{std:("simulation"), std:("simulation")}, 
        object::pair{std:("nodes"), std:("nodes")}, 
        object::pair{std:("links"), std:("links")}
    };
}

