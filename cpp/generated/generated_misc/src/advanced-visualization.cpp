#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics/advanced-visualization.h"

std::shared_ptr<ChartConfiguration> AdvancedVisualization::generateSegmentationSunburst(array<any> segments)
{
    return object{
        object::pair{std::string("type"), std::string("sunburst")}, 
        object::pair{std::string("data"), AdvancedVisualization::transformSegmentData(segments)}, 
        object::pair{std::string("options"), object{
            object::pair{std::string("responsive"), true}, 
            object::pair{std::string("plugins"), object{
                object::pair{std::string("legend"), object{
                    object::pair{std::string("position"), std::string("right")}
                }}, 
                object::pair{std::string("tooltip"), object{
                    object::pair{std::string("callbacks"), object{
                        object::pair{std::string("label"), [=](auto ctx) mutable
                        {
                            return string_empty + ctx["raw"]["value"] + std::string(" users");
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
        object::pair{std::string("type"), std::string("scatter")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("datasets"), array<object>{ object{
                object::pair{std::string("label"), std::string("Engagement Predictions")}, 
                object::pair{std::string("data"), predictions->map([=](auto p) mutable
                {
                    return (object{
                        object::pair{std::string("x"), p["timestamp"]}, 
                        object::pair{std::string("y"), p["score"]}, 
                        object::pair{std::string("r"), p["confidence"] * 10}
                    });
                }
                )}, 
                object::pair{std::string("backgroundColor"), std::string("rgba(75, 192, 192, 0.5)")}
            } }}
        }}, 
        object::pair{std::string("options"), object{
            object::pair{std::string("scales"), object{
                object::pair{std::string("y"), object{
                    object::pair{std::string("beginAtZero"), true}, 
                    object::pair{std::string("title"), object{
                        object::pair{std::string("display"), true}, 
                        object::pair{std::string("text"), std::string("Engagement Score")}
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
    auto simulation = d3->forceSimulation(nodes)->force(std::string("link"), d3->forceLink(links)->id([=](auto d) mutable
    {
        return d["id"];
    }
    ))->force(std::string("charge"), d3->forceManyBody())->force(std::string("center"), d3->forceCenter(width / 2, height / 2));
    return object{
        object::pair{std::string("simulation"), std::string("simulation")}, 
        object::pair{std::string("nodes"), std::string("nodes")}, 
        object::pair{std::string("links"), std::string("links")}
    };
}

