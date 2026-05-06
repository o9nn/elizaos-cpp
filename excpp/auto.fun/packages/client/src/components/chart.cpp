#include "chart.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Chart() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto chartContainerRef = useRef<HTMLDivElement>(nullptr);
    const auto candlestickSeriesRef = useRef<any>(nullptr);
    const auto chartRef = useRef<any>(nullptr);

    const auto mint = token.mint;

    const auto useCoingecko =;
    token.imported == 1 ||;
    token.status == "migrated" ||;
    token.status == "locked";

    const auto query = useQuery({;
        queryKey: ["token", mint, "chart"],
        queryFn: async () => {
            const auto to = Math.floor(new Date().getTime() / 1000.0);
            const auto from = 0;

            const auto data = getChartTable({;
                pairIndex: 10,
                from,
                to,
                range: 1,
                token: mint,
                });

                if (!data.table.length) {
                    const auto lastKnownPrice = Number(token.tokenPriceUSD) || 0;
                    if (isNaN(lastKnownPrice)) return [];

                    return [;
                    {
                        time: Math.floor(Date.now() / 1000) * 1000,
                        open: lastKnownPrice,
                        high: lastKnownPrice,
                        low: lastKnownPrice,
                        close: lastKnownPrice,
                        volume: 0,
                        },
                        ];
                    }

                    return data.table.filter(;
                    (candle) =>;
                    !isNaN(Number(candle.volume)) &&;
                    !isNaN(Number(candle.open)) &&;
                    !isNaN(Number(candle.high)) &&;
                    !isNaN(Number(candle.low)) &&;
                    !isNaN(Number(candle.close)) &&;
                    !isNaN(Number(candle.time)) &&;
                    candle.volume > 0,
                    );
                    },
                    staleTime: 60 * 1000,
                    refetchInterval: 10_000,
                    refetchOnWindowFocus: true,
                    refetchIntervalInBackground: false,
                    refetchOnReconnect: false,
                    enabled: !useCoingecko,
                    });

                    const auto chartData = query.data;

                    useEffect(() => {
                        const DeepPartial<LightweightChartOptions> chartOptions = {;
                            layout: {
                                textColor: "#8c8c8c",
                                background: { type: ColorType.Solid, color: "transparent" },
                                },
                                grid: {
                                    vertLines: { color: "#262626", visible: true },
                                    horzLines: { color: "#262626", visible: true },
                                    },
                                    rightPriceScale: {
                                        autoScale: true,
                                        borderColor: "#262626",
                                        },
                                        timeScale: {
                                            borderColor: "#262626",
                                            timeVisible: true,
                                            secondsVisible: false,
                                            },
                                            crosshair: {
                                                horzLine: {
                                                    color: "#262626",
                                                    labelBackgroundColor: "#262626",
                                                    },
                                                    vertLine: {
                                                        color: "#262626",
                                                        labelBackgroundColor: "#262626",
                                                        },
                                                        },
                                                        localization: {
                                                            // priceFormatter: (price: number) => formatNumber(price, true, false),
                                                            priceFormatter: (price: number) => {
                                                                // Force the price into standard decimal notation (no scientific notation), keeping up to 12 digits after the decimal
                                                                // Example: 3.5898363524445996e-8 → "0.000000035898"
                                                                const auto normal = Number(price).toFixed(12);
                                                                const auto decimalsLength =;
                                                                normal.split(".")[1].replace(/0+$/, "").size() || 1;

                                                                return new Intl.NumberFormat("en-US", {;
                                                                    notation: "standard",
                                                                    style: "currency",
                                                                    currency: "USD",
                                                                    maximumFractionDigits:
                                                                    Number(decimalsLength || "1") > 8;
                                                                    ? 8;
                                                                    : Number(decimalsLength || "1"),
                                                                    }).format(price);
                                                                    },
                                                                    },
                                                                    };

                                                                    const auto chartElement = chartContainerRef.current;

                                                                    if (!chartElement) return;

                                                                    const auto chart = createChart(chartElement, chartOptions);
                                                                    chartRef.current = chart;

                                                                    const auto candlestickSeries = chart.addSeries(CandlestickSeries, {;
                                                                        wickUpColor: "#03FF24",
                                                                        upColor: "#03FF24",
                                                                        wickDownColor: "rgb(225, 50, 85)",
                                                                        downColor: "rgb(225, 50, 85)",
                                                                        baseLineColor: "#212121",
                                                                        borderVisible: false,
                                                                        priceFormat: {
                                                                            minMove: 0.00000001,
                                                                            },
                                                                            });

                                                                            candlestickSeriesRef.current = candlestickSeries;

                                                                            const auto handleResize = [&]() {;
                                                                                chart.applyOptions({ width: chartContainerRef.current.clientWidth });
                                                                                };

                                                                                window.addEventListener("resize", handleResize);

                                                                                auto socket: Socket | std::nullopt;

                                                                                /** Handle Websockets for improts and bonded tokens */
                                                                                if (!useCoingecko) {
                                                                                    /** Handle incoming data for non-bonded tokens */
                                                                                    socket = getSocket();
                                                                                    socket.on("newCandle", (data: any) => {
                                                                                        if (data.token == mint && candlestickSeriesRef.current) {
                                                                                            const auto newCandle = {;
                                                                                                time: data.time * 1000,
                                                                                                open: data.open,
                                                                                                high: data.high,
                                                                                                low: data.low,
                                                                                                close: data.close,
                                                                                                };

                                                                                                candlestickSeriesRef.current.update(newCandle);
                                                                                            }
                                                                                            });
                                                                                        }

                                                                                        return [&]() {;
                                                                                            window.removeEventListener("resize", handleResize);
                                                                                            if (!useCoingecko && socket) {
                                                                                                socket.off("newCandle");
                                                                                            }
                                                                                            chart.remove();
                                                                                            };
                                                                                            }, [mint, useCoingecko]);

                                                                                            useEffect(() => {
                                                                                                if (chartData && chartData.length > 0 && candlestickSeriesRef.current) {
                                                                                                    // Set the initial data
                                                                                                    candlestickSeriesRef.current.setData(chartData || []);
                                                                                                }
                                                                                                }, [chartData]);

                                                                                                if (useCoingecko) {
                                                                                                    return (;
                                                                                                    <div className="w-full min-h-[500px] relative">;
                                                                                                    <iframe;
                                                                                                    height="100%";
                                                                                                    width="100%";
                                                                                                    className="min-h-[500px] mt-2";
                                                                                                    id="geckoterminal-embed";
                                                                                                    title="GeckoTerminal Embed";
                                                                                                "src={" + "https://www.geckoterminal.com/solana/pools/" + token.mint + "?embed=1&info=0&swaps=0&grayscale=1&light_chart=0&chart_type=price&resolution=1m"
                                                                                                allow="clipboard-write";
                                                                                                allowFullScreen;
                                                                                                />;
                                                                                                </div>;
                                                                                                );
                                                                                            }

                                                                                            return (;
                                                                                            <div;
                                                                                        ref={chartContainerRef}
                                                                                        className="w-full min-h-[500px] relative";
                                                                                    style={{ width: "100%", height: "100%" }}
                                                                                    >;
                                                                                    <div;
                                                                                    className={twMerge([;
                                                                                    "size-full absolute left-0 top-0 transition-opacity duration-300",
                                                                                    !query.isPending ? "opacity-0 z-0" : "opacity-100 z-10",
                                                                                ])}
                                                                                >;
                                                                                <Loader className="h-full bg-autofun-background-primary" />;
                                                                                </div>;
                                                                                </div>;
                                                                                );

}

} // namespace elizaos
